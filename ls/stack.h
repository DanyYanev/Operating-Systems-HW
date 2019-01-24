//--------------------------------------------
//NAME: Dany Yanev
//CLASS: XIA
//NUMBER: 08
//PROBLEM: ls
//FILE NAME: ls.h
//FILE PURPOSE: Normal dynamic array containing char *
//--------------------------------------------

#ifndef STACK_H_
#define STACK_H_

#include "helper.h"

struct stack
{
	char ** paths;
	size_t size;
	size_t _capacity;
};

int stack_init(struct stack* stack)
{
	stack->_capacity = 10;
	stack->size = 0;
	stack->paths = malloc(sizeof(char *) * stack->_capacity);

	return stack->paths == NULL;
}

void stack_push(struct stack* stack, char * element)
{
	if (stack->size == stack->_capacity)
	{
		stack->paths = realloc(stack->paths, sizeof(char *) * (stack->_capacity *= 2));
	}

	stack->paths[stack->size++] = element;
}

void stack_destroy(struct stack* stack)
{
	size_t tmp = stack->size;
    stack->size = 0;
	stack->_capacity = 0;

	for (size_t i = 0; i < tmp; ++i)
	{
		free(stack->paths[i]);
	}

	free(stack->paths); // double free err
}

#endif
