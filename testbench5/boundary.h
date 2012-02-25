#ifndef _boundary_
#define _boundary_

typedef struct bcond1D
{
  double *time;
  double *value;
  double *barrier; 
  int bccount;
}cashflows1d;

typedef struct bcond2D
{
  double **times;
  double **values;
  double **barrier;
}boundaryconditions2D;

void initialize_cashflows1d(cashflows1d* bc,int n);
#endif
