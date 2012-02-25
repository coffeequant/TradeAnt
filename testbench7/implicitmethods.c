#include <stdio.h>
#include <math.h>
#include <gsl/gsl_linalg.h>
double max(double a,double b)
{
	return (a>b)?a:b;
}

int main()
{
	double prices[40000];
	double dt = 0.01;
	double ds = 1;
	double coeffs[40000];
	//call payoff
	double S[200];
	double rho = 0.1;
	double sigma = 0.20;
	//
	int i,j,k;
	for(i=0;i<200;i++) S[i] = i * ds;
	double r = 0.05;
	double A[40000];
	double B[40000];
	double C[40000];
	double D[40000];
	double E[40000];
	double F[40000];
	double G[40000];
	double H[40000];
	double I[40000];
	for(i=1;i<199;i++)
	{
		for(j=1;j<199;j++)
		{
			A[i-1+(j)*200-1] = (dt*rho*pow(sigma,2))/(4*ds*ds);
			B[i-1+j*200] = (-r * dt * S[i]/(2*ds)) + (dt/(ds*ds))*(1/2)*(sigma*sigma*S[i]*S[i]);
			C[i-1+(j)*200+1] = (-dt*rho*pow(sigma,2))/(4*ds*ds);
			D[i+j*200-1] = (-r * dt * S[j] / (2*ds)) + (dt/(ds*ds))*0.5*sigma*sigma*S[j]*S[j];
			E[i+j] = 1-(dt/(ds*ds))*sigma*sigma*S[i]*S[i] - (dt/(ds*ds))*sigma*sigma*S[j]*S[j];
			F[i+j*200+1] = r*S[j]*dt/(2*ds) + 0.5*((dt)/(ds*ds))*sigma*sigma*S[j]*S[j];
			G[i+1+200*j-1] = -rho*sigma*sigma*dt/(4*ds*ds);
			H[i+1+200*j] = dt*4*S[i]/(2*ds) + 0.5*(dt/(ds*ds)) * sigma*sigma*S[i]*S[i];
			I[i+1+200*j+1] = dt*rho*sigma*sigma/(4*ds*ds);
		}
	}
	
	
	for(i=0;i<40000;i++)
	{
		coeffs[i] = 0;
		prices[i] = 0;
	}

	for(i=0;i<200;i++)
	{
		for(j=0;j<200;j++)
		{
			prices[i+200*j] = max(max(i,j)-100.0,0.0);
			if(i==0 || j==0) coeffs[i+j*200] = (1-r*dt);
			if(i==199 || j==199)
			{
				coeffs[i+200*j-3] = -1;		
				coeffs[i+200*j-2] = 2;
				coeffs[i+200*j-1] = -1;
			}	

		}
	
	}
				

	for(i=1;i<199;i++)
		for(j=i;j<(i+1);j++)
		{
			coeffs[i+200*j] = A[i];
			coeffs[i+200*j+1] = B[i];
			coeffs[i+200*j+2] = C[i];
			coeffs[i+200*j+3] = D[i];
			coeffs[i+200*j+4] = E[i];
			coeffs[i+200*j+5] = F[i];
			coeffs[i+200*j+6] = G[i];
			coeffs[i+200*j+7] = H[i];
			coeffs[i+200*j+8] = I[i];
		}

	gsl_matrix_view m = gsl_matrix_view_array(coeffs,200,200);
	gsl_vector_view b = gsl_vector_view_array(prices,200);

	gsl_vector *x = gsl_vector_alloc(200);
	gsl_permutation *p = gsl_permutation_alloc(200);
	
	int s;
	gsl_linalg_LU_decomp(&m.matrix,p,&s);
	
	gsl_linalg_LU_solve(&m.matrix,p,&b.vector,x);
	exit(0);
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
