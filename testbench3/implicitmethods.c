#include <stdio.h>
#include <math.h>
#include <gsl/gsl_linalg.h>
double max(double a,double b)
{
	return (a>b)?a:b;
}

int main()
{
	double prices[200];
	double dt = 0.01;
	double ds = 1;
	double coeffs[40000];
	//call payoff
	prices[199] = 0;
	//
	int i,j,k;
	double r = 0.05;
	double A[200];
	double B[200];
	double C = -1*r * dt;
	for(i=0;i<199;i++) {
	//cash flow section...
		prices[i] = max((i+1)*ds - 100.0,0.0);
		A[i] = (dt/(ds*ds))*0.5*0.20*0.20*((i+1)*ds*(i+1)*ds);
		B[i] =  r*(i+1)*ds*dt/(2*ds);
	}
	
	
	for(i=0;i<40000;i++)
		coeffs[i] = 0;
	
	coeffs[0] = (1-r*dt);
	coeffs[200*200-3] = -1;
	coeffs[200*200-2] = 2;
	coeffs[200*200-1] = -1;

	for(i=1;i<199;i++)
		for(j=i;j<(i+1);j++)
		{
			coeffs[j+200*i-1] = -A[i]+B[i];
			coeffs[j+200*i] = 1+2*A[i]-C;
			coeffs[j+200*i+1] = -1*A[i]-B[i];
		}

	gsl_matrix_view m = gsl_matrix_view_array(coeffs,200,200);
	gsl_vector_view b = gsl_vector_view_array(prices,200);

	gsl_vector *x = gsl_vector_alloc(200);
	gsl_permutation *p = gsl_permutation_alloc(200);
	
	int s;
	gsl_linalg_LU_decomp(&m.matrix,p,&s);
	gsl_linalg_LU_solve(&m.matrix,p,&b.vector,x);
	for(j=1;j<100;j++)
	{
		for(i=0;i<200;i++)
		{
			prices[i] = x->data[i];
//			printf("%.2f\t",prices[i]);
		}
		gsl_vector_view cc = gsl_vector_view_array(prices,200);
		gsl_vector_set_zero(x);
		prices[199] = 0;
		gsl_linalg_LU_solve(&m.matrix,p,&cc.vector,x);
	}

	printf("%.2f",x->data[99]);
	gsl_permutation_free(p);

	gsl_vector_free(x);
printf("DDA");

	return 0;
}
