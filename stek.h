#pragma once

typedef int stack_data;

enum Guards
{
	FRONT = 0xBADBEDA,
	BACK = 0xBACBEDA
};

enum parameters
{
	MAX_SIZE = 100,
	CAPACITY_MULTIPLIER = 2
};

enum modes
{
	ExtendMode = 1,
	ShrinkMode = 0
};

enum stek_errors
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
	char* err_name;

	long int hash_func;

	int BackGuard;
};

void StackBuild(struct Stack* dis);
void StackPush(struct Stack* dis, stack_data element);
double StackPop(struct Stack* dis);
void StackDelete(struct Stack* dis);
void StackResize(struct Stack* dis, int mode);
int StackOK(struct Stack* dis);
void StackDump(struct Stack* dis);
long int HashFuncStack(struct Stack* dis);
long int HashFuncData(struct Stack* dis);