
typedef struct bcond1D
{
  double *time;
  double *value;
  double *barrier; 
  int bccount;
}boundaryconditions1D;

typedef struct bcond2D
{
  double **times;
  double **values;
  double **barrier;
}boundaryconditions2D;

void initialize_boundaryconditions1D(boundaryconditions1D* bc,int n);
