#ifndef _boundary_
#define _boundary_
#include "boundary.h"
#endif


void initialize_boundaryconditions1D(boundaryconditions1D* bc,int n)
{
	bc->time = (double*)malloc(sizeof(double)*n);
	bc->value = (double*)malloc(sizeof(double)*n);
	bc->barrier =(double*)malloc(sizeof(double)*n);
	bc->bccount = n;
}
