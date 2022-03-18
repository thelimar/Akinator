#ifndef DEBUG
#define ASSERT_OK
#define DBG if(0)
#else
#define ASSERT_OK if(!StackOK(dis)) {\
                                        printf("Assertion failed on line %d\n", __LINE__);\
                                        StackDump(dis); abort();\
                                    }
#define DBG
#endif

#define stack_data_format "%d"

typedef int stack_data;

#define _CRT_SECURE_NO_WARNINGS

#define GetEnumName(enum_const) #enum_const

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <math.h>

enum Guards
{
	FRONT = 0xBADBEDA,
	BACK  = 0xBACBEDA
};

enum parameters
{
	MAX_SIZE = 100,
	CAPACITY_MULTIPLIER = 2,
	POISON = -999999998,
};

enum modes
{
	ExtendMode = 1,
	ShrinkMode = 0
};

enum errors
{
	ErrorDeadGuardFront = 1,
	ErrorDeadGuardBack = 2,
	ErrorNullPtr = 3,
	ErrorOversize = 4,
	ErrorUndersize = 5,
	ErrorNegativeCapacity = 6,
	ErrorDeadGuardFrontMini = 7,
	ErrorDeadGuardBackMini = 8,
	ErrorHashStack = 9,
	ErrorHashData = 10,
};

struct Stack
{
	int FrontGuard;

	int FrontGuardMini;
	stack_data* data;
	int BackGuardMini;

	int count;
	int capacity;

	int err_num;
	const char* err_name;

	long int hash_func;

	int BackGuard;
};

void StackBuild(struct Stack* dis);
void StackPush(struct Stack* dis, stack_data element);
stack_data StackPop(struct Stack* dis);
void StackDelete(struct Stack* dis);
void StackResize(struct Stack* dis, int mode);
int StackOK(struct Stack* dis);
void StackDump(struct Stack* dis);
long int HashFuncStack(struct Stack* dis);
long int HashFuncData(struct Stack* dis);

void StackBuild(struct Stack* dis)
{
	assert(dis);

	dis->FrontGuard = FRONT;
	dis->BackGuard = BACK;

	dis->FrontGuardMini = FRONT;
	dis->BackGuardMini = BACK;

	dis->capacity = MAX_SIZE;

	size_t buf_size = sizeof(stack_data) * dis->capacity + sizeof(int) * 2 + sizeof(long int);
	dis->data = (stack_data*) calloc (1, buf_size); 
 
    DBG printf("CTOR original data = [%p]\n", dis->data);

	dis->data = (stack_data*) ((char*) dis->data + sizeof(int));
	DBG printf("CTOR adjusted data = [%p]\n", dis->data);

	assert(dis->data);

	*(int*) ((char*)dis->data                                  - sizeof(int))      = dis->FrontGuardMini;	
	*(int*) ((char*)dis->data + dis->capacity * sizeof(stack_data) + sizeof(long int)) = dis->BackGuardMini;

	DBG printf("good pointer = [%p]\n", (int*)((char*)dis->data - sizeof(int))), 
		(int*)((char*)dis->data + dis->capacity * sizeof(stack_data) + sizeof(long int));
	DBG printf("hash pointer - [%p]\n", (int*)((char*)dis->data + dis->capacity * sizeof(stack_data)));
	DBG printf("bad pointer = [%p], [%p]\n", dis->data - sizeof(int), dis->data + dis->capacity * sizeof(stack_data));

	for (int i = 0; i < MAX_SIZE; i++) dis->data[i] = POISON;
	dis->count = 0;

	*(long int*) ((char*)dis->data + dis->capacity * sizeof(stack_data)) = HashFuncData(dis);

	dis->hash_func = HashFuncStack(dis);

	ASSERT_OK
}

void StackPush(struct Stack* dis, stack_data element)
{
	assert(dis);

	ASSERT_OK

	if (dis->count == dis->capacity)
	{
		StackResize(dis, ExtendMode);
	}

	dis->data[dis->count] = element;
	dis->count++;

	dis->hash_func = HashFuncStack(dis);

	*(long int*) ((char*)dis->data + dis->capacity * sizeof(stack_data)) = HashFuncData(dis);

	ASSERT_OK
}

stack_data StackPop(struct Stack* dis)
{
	assert(dis);

	if (dis->count == 0) dis->count--;
	dis->hash_func = HashFuncStack(dis);

	ASSERT_OK

	if (dis->capacity / dis->count == CAPACITY_MULTIPLIER * CAPACITY_MULTIPLIER)
	{
		StackResize(dis, ShrinkMode);
	}

	stack_data popped = dis->data[--dis->count];
	dis->data[dis->count + 1] = POISON;

	dis->hash_func = HashFuncStack(dis);

	*(long int*)((char*)dis->data + dis->capacity * sizeof(stack_data)) = HashFuncData(dis);

	ASSERT_OK

	return popped;
}

void StackDelete(struct Stack* dis)
{
	DBG printf("DTOR adjusted data = [%p]\n", dis->data);
	dis->data = (stack_data*)((char*)dis->data - sizeof(int));
	DBG printf("DTOR original data = [%p]\n", dis->data);
	
	assert(dis);

	free(dis->data);
	dis->data = NULL;

	dis->count = 0;
	dis->capacity = 0;
}

void StackResize(struct Stack* dis, int mode)
{
	assert(dis);

	ASSERT_OK

	if (mode == ExtendMode)
	{
		size_t buf_size = CAPACITY_MULTIPLIER * dis->capacity * sizeof(stack_data) + 2 * sizeof(int) + sizeof(long int);
		dis->data = (stack_data*) realloc((char*) dis->data - sizeof(int), buf_size);

		DBG printf("Resize original data = [%p]\n", dis->data);
		
		dis->data = (stack_data*)((char*)dis->data + sizeof(int));

		DBG printf("Resize adjusted data = [%p]\n", dis->data);

		dis->capacity = dis->capacity * CAPACITY_MULTIPLIER;

		assert(dis->data);

		for (int i = dis->count; i < dis->capacity; i++) dis->data[i] = POISON;

		*(int*)((char*)dis->data - sizeof(int)) = dis->FrontGuardMini;
		*(int*)((char*)dis->data + dis->capacity * sizeof(stack_data) + sizeof(long int)) = dis->BackGuardMini;

	}

	else if (mode == ShrinkMode)
	{
		size_t buf_size = sizeof(stack_data) * dis->capacity / CAPACITY_MULTIPLIER + 2 * sizeof(int) + sizeof(long int);
		dis->data = (stack_data*) ((char*) realloc((char*) dis->data - sizeof(int), buf_size) + sizeof(int));
		dis->capacity = dis->capacity / CAPACITY_MULTIPLIER;

		assert(dis->data);

		*(int*)((char*)dis->data - sizeof(int)) = dis->FrontGuardMini;
		*(int*)((char*)dis->data + dis->capacity * sizeof(stack_data) + sizeof(long int)) = dis->BackGuardMini;

	}

	dis->hash_func = HashFuncStack(dis);

	*(long int*)((char*)dis->data + dis->capacity * sizeof(stack_data)) = HashFuncData(dis);

	ASSERT_OK
}

int StackOK(struct Stack* dis)
{
	assert(dis);

	if(dis->FrontGuard != FRONT)
	{
		dis->err_num = ErrorDeadGuardFront;
		dis->err_name = GetEnumName(ErrorDeadGuardFront);

		return 0;
	}

	else if(dis->BackGuard != BACK)
	{
		dis->err_num = ErrorDeadGuardBack;
		dis->err_name = GetEnumName(ErrorDeadGuardBack);

		return 0;
	}

	else if (dis->hash_func != HashFuncStack(dis))
	{
		dis->err_num = ErrorHashStack;
		dis->err_name = GetEnumName(ErrorHashStack);

		return 0;
	}

	else if (*(int*)((char*)dis->data + dis->capacity * sizeof(stack_data)) != HashFuncData(dis))
	{
		dis->err_num = ErrorHashData;
		dis->err_name = GetEnumName(ErrorHashData);

		return 0;
	}

	else if(!dis->data)
	{
		dis->err_num = ErrorNullPtr;
		dis->err_name = GetEnumName(ErrorNullPtr);

		return 0;
	}

	else if (dis->FrontGuardMini != *(int*)((char*)dis->data - sizeof(int)))
	{
		dis->err_num = ErrorDeadGuardFrontMini;
		dis->err_name = GetEnumName(ErrorDeadGuardFrontMini);

		return 0;
	}

	else if (dis->BackGuardMini != *(int*)((char*)dis->data + dis->capacity * sizeof(stack_data) + sizeof(long int)))
	{
		dis->err_num = ErrorDeadGuardBackMini;
		dis->err_name = GetEnumName(ErrorDeadGuardBackMini);

		return 0;
	}

	else if(dis->count > dis->capacity)
	{
		dis->err_num = ErrorOversize;
		dis->err_name = GetEnumName(ErrorOversize);

		return 0;
	}
	else if(dis->count < 0)
	{
		dis->err_num = ErrorUndersize;
		dis->err_name = GetEnumName(ErrorUndersize);

		return 0;
	}
	else if(dis->capacity < 0)
	{
		dis->err_num = ErrorNegativeCapacity;
		dis->err_name = GetEnumName(ErrorNegativeCapacity);

		return 0;
	}
	else return 1;

}

void StackDump(struct Stack* dis)
{
	FILE* logs_output = fopen("logs.txt", "a");

	if (dis->err_num == ErrorDeadGuardFront || dis->err_num == ErrorDeadGuardBack)
		fprintf(logs_output, "Guards are dead, so it's scary to read after them.\n");
	else
	{
		fprintf(logs_output, "Stack (Error #%d - %s) [%p]\n", dis->err_num, dis->err_name, &dis);
		fprintf(logs_output, "\t{\n\tsize = %d\n\tcapacity = %d\n\tdata[%p]\n\t\t{\n", dis->count, dis->capacity, dis->data);

		if (dis->err_num == ErrorNullPtr) 
			fprintf(logs_output, "\t\tData is corrupted. Unable to read.\n");

		else if (dis->err_num == ErrorDeadGuardFrontMini || dis->err_num == ErrorDeadGuardBackMini)
			fprintf(logs_output, "Guards are dead, so it's scary to read data after them.\n");
		else
		{
			int counter = 0;
			while (!isnan((double) dis->data[counter]))
			{
				fprintf(logs_output, "\t\t*[%d] = %d\n", counter, dis->data[counter]);  //stack_data_format
				counter++;
			}

			while (counter < dis->capacity)
			{
				fprintf(logs_output, "\t\t[%d] = %d (POISON!)\n", counter, dis->data[counter]);  //stack_data_format
				counter++;
			}

			fprintf(logs_output, "\t\t}\n");
		}
		fprintf(logs_output, "\t}\n");
	}

	fclose(logs_output);
}

long int HashFuncStack(struct Stack* dis)
{
	long int hash_sum = 0;

	for (int i = 0; i < sizeof(*dis) - sizeof(dis->BackGuard) - sizeof(dis->hash_func); i++)
	{
		hash_sum = (hash_sum << 1) + (int) *((char*) dis + i);
		DBG printf("hash stek: %ld\n", (int) *((char*)dis + i));
	}

	return hash_sum;
}

long int HashFuncData(struct Stack* dis)
{
	long int hash_sum = 0;

	for (int i = 0; i < dis->capacity; i++)
	{
		hash_sum = (hash_sum << 1) + (long int) dis->data[i];
		DBG printf("hash data: %ld\n", hash_sum);
	}

	return hash_sum;
}