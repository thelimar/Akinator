#pragma once


#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

//#define DEBUG

#ifndef DEBUG
#define DBG if(0)
#else
#define DBG
#endif

typedef char* TreeData;

enum parametors
{
	MAX_DATA_LEN = 20,
};

enum errors
{
	SIZE_ERROR = 1,
	RELATION_ERROR = 2,
	ROOT_NULL = 3,
};

struct Node
{
	TreeData data;
	struct Node* left;
	struct Node* right;
	struct Node* parent;
};

struct Tree
{
	struct Node* root;
	int size;
};

struct Tree* TreeBuild(TreeData root_data);
int AddNodeLeft(struct Tree* dis, struct Node* parent, TreeData node_data);
int AddNodeRight(struct Tree* dis, struct Node* parent, TreeData node_data);
void TreeGraphicDump(struct Tree* dis);
void DrawNodes(FILE* output, struct Node* node);
void NodeDestruct(struct Node* dis);
void TreeDestruct(struct Tree* dis);
int TreeVerificator(struct Tree* dis);
void TreeParentCheck(struct Node* dis, int* parent_ok);
void TreeTextDump(struct Node* dis);
void TreeCount(struct Node* dis, int* nNodes);