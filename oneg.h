#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

char* PoemEnter(int* nChars, int* nLines, FILE* source);
void PoemWriteSource(char* poem_in, int nChars, FILE* output);
