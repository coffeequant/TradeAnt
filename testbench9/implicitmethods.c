#include <stdio.h>
#include <math.h>
double max(double a,double b)
{
	return (a>b)?a:b;
}

int main()
{
	double prices[200][1000];
	//upstream downstream method

	double dt = 0.001;
	int i,j,k;
	int ds = 1.0;
	int nts = 1/dt;
	int nas = 200;
	double Au[200];
	double Bu[200];
	double D;	
	double Eu[200];	
	double Fu[200];	
	double r = 0.00;
	
	double Bd[200];
	double Ed[200];	
	double Fd[200];	
	double Ad[200];
	

	double sigma = 0.20;
	D = 1-r*dt;

	for(i=0;i<200;i++)		
	{
		Au[i] = 1 - r*(i*ds) * dt/ds + 0.5*sigma*sigma*i*ds*i*ds*dt/(ds*ds);
		Bu[i] = 0.5*sigma*sigma*i*ds*i*ds*dt/(ds*ds);
		Eu[i] = -r*i*ds*dt/ds;
		Fu[i] = 0.5*sigma*sigma*i*ds*i*ds*dt/(ds*ds);		
		prices[i][0] = max(i*ds-100.0,0.0);
		
		Ad[i] = Au[i];
		
	//	printf("%.2f \t %.2f \t%.2f \t %.2f\n",A[i],B[i],E[i],F[i]);
	}
	
	for(j=1;j<nts;j++)
	{
		if(j > 0)
		{
			prices[0][j] = prices[0][j-1]*(1-r*dt);			
		}
	
		if (j%2==0)
		{
		
			//upstream
		for(i=1;i<nas-1;i++)
			{
					prices[i][j] = (Bu[i]*prices[i-1][j]+prices[i][j-1]*D+prices[i-1][j-1]*Eu[i]+(prices[i+1][j-1]-prices[i][j-1])*Fu[i])/Au[i];
		//	printf("%d \t %.2f\n",(i*ds),prices[i][1]);
			}
			prices[nas-1][j] = 2 *prices[nas-2][j] - prices[nas-3][j];
		}
		else
		{	//downstream
		
		prices[nas-1][j] = prices[nas-1][j-1];
		for(i=nas-2;i>0;i--)
			{
						
		prices[i][j] = (Bu[i]*prices[i+1][j] + prices[i][j-1]*D+prices[i-1][j-1]*Eu[i]+Fu[i]*(prices[i-1][j-1]-prices[i][j-1]))/Au[i];
	//		printf("DDDDDDDDDDDD");
			};

		}		
	}
	for(i=0;i<200;i++)
	printf("%d %.5f\n",(i)*ds,prices[i][nts-1]);		
	return 0;
}
