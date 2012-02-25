#include "boundary.h"


void initialize_cashflows1d(cashflows1d* bc,int n)
{
	bc->time = (double*)malloc(sizeof(double)*n);
	bc->value = (double*)malloc(sizeof(double)*n);
	bc->barrier =(double*)malloc(sizeof(double)*n);
	bc->bccount = n;
}
