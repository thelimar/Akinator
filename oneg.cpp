#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

char* PoemEnter(int* nChars, int* nLines, FILE* source)
{

	fseek(source, '\0', SEEK_END);
	long source_size = ftell(source);
	rewind(source);

	char* text_buffer = (char*)calloc(source_size + 1, sizeof(char));
	if (!text_buffer)
	{
		return NULL;
	}

	if (source_size * sizeof(char) != fread(text_buffer, sizeof(char), source_size, source))
	{
		return NULL;
	}

	for (int index = 0; index < source_size; index++)
	{
		if (text_buffer[index] == '\n')
		{
			text_buffer[index] = '\0';
			(*nLines)++;
		}
	}

	*nChars = source_size + 1;
	(*nLines)++;

	fclose(source);

	return text_buffer;
}

void PoemWriteSource(char* poem_in, int nChars, FILE* output)
{

	char message[] = "Ne bey, original ne isporchen:\n\n";
	fputs(message, output);

	for (int i = 0; i < nChars; i++)
	{
		fprintf(output, "%s", poem_in + i);
		i = i + strlen(poem_in + i);
	}
}