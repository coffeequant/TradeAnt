#include <stdio.h>

#include "model.h"
#include <math.h>
#include <gsl/gsl_errno.h>

//this file combines all the data and is the main pricing engine
//the non standard PDE would allow other than standard variables to have two extra terms....
//problem is how to decide the order of loops....
//first is always time...but depending on boundary conditions out of 3 which one's come first....


void initialize_blackscholes1d(blackscholes1d *bs)
{
	  bs->solve = solvebs;
	  bs->set_vol_surface = _set_vol_surface;
	  bs->apply_boundary_conditions = _apply_boundary_conditions;
	  bs->numberofsteps = 200;
	  bs->stepsize = 1;
}

void _apply_boundary_conditions(blackscholes1d *bs,boundaryconditions1D modelbc)
{
  	bs->bc = modelbc;
}
void _set_vol_surface(blackscholes1d *bs,volsurface v)
{
	bs->_blackscholesvol = v;

}
 
results solvebs(blackscholes1d* bs)
 {
    double dt = 0.9/(4*0.20*0.20*100*100);
    double expiry = 3.0; //clearn fix - animesh
    int nts = (int)(expiry / dt);
    double ds = bs->stepsize;
    double **prices;
    int i,j;
    //2D array using pointers
   prices = (double**)malloc(sizeof(double*)*(bs->numberofsteps));
    for(i=0;i<bs->numberofsteps;i++)
      prices[i] = (double*)malloc(sizeof(double)*nts);


    //initialize prices to size nts and numberofsteps - animesh
    double intrate = 0.0;   //temp - animesh
    double volatility = 0.20; // temp - animesha
    double divd = 0.0;
    double delta,gamma,theta;
    int b = 0;
    for(j=0;j<nts-1;j++)
      {
	for(b=0;b<bs->bc.bccount;b++)
	{
		if((nts-j)*dt >= bs->bc.time[b])
		{
			for(i=0;i<bs->numberofsteps;i++)
				if(i*ds >= bs->bc.barrier[b])
					prices[i][j] = bs->bc.value[b];
		}
	}
	for(i=1;i<bs->numberofsteps-1;i++)
	  {
	    //apply boundary conditionsa
	    volatility = bs->_blackscholesvol.get_vol_with_reftime(&(bs->_blackscholesvol),((nts-j)*dt),(i*ds));
	//    printf("%.2f",volatility);
//awesome - damn awesome - animesh
	    delta = (prices[i+1][j] - prices[i-1][j])/(2*ds);
	    gamma = (prices[i+1][j]+prices[i-1][j]-2*prices[i][j])/(ds*ds);
	    theta = -0.5*volatility*volatility*i*ds*i*ds*gamma -(intrate-divd)*i*ds*delta + intrate*prices[i][j];
	    prices[i][j+1]=prices[i][j]-dt*theta;
	
	  }
	prices[0][j+1] = prices[0][j]*(1-intrate*dt);
	prices[bs->numberofsteps-1][j+1] = 2 *prices[bs->numberofsteps-2][j+1]-prices[bs->numberofsteps-3][j+1];
      }
	//printf("1HERE");exit(0);
 

results _vanilla;
_vanilla.prices = prices;
return _vanilla;
    //populate results object
    //send it back along with basic greeks
}
//how to link the model chosen with bounday conditions
//Common points in boundary condition --
//1. Each boundary condition is applied at a specific time
//2. It has a specific payoff so why not keep it in any array
//3. This array should syncrhonize with the time step and space step chosen
//4. This should complete our generic model of the model and FD Engine / Calculator! Any loopholes???
//5.

