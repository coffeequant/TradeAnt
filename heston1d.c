/*
 Copyright (C) 2012 Animesh Saxena

 This file is part of TradeAnt, a free-software/open-source library
 for traders and practical quantitative analysts

 TradeAnt is free software: you can redistribute it. Use it at your own risk
*/

#include <stdio.h>
#include <tradeant/heston1d.h>
#include <math.h>

#include <gsl/gsl_errno.h>
#include <gsl/gsl_linalg.h>

//this file combines all the data and is the main pricing engine


void initialize_heston1d(heston1d *hs)
{
	  hs->solve = _solveheston1d;
	  hs->set_rates = _set_ratesheston1d;
	  hs->apply_cashflow = NULL;
	  hs->apply_coupon = NULL;
      hs->set_model_parameters = _set_model_parameters_heston1d;
	  hs->_eps.dimension = 2;
	  hs->set_calibration_parameters = _set_calibration_parametersheston;
}

void _set_calibration_parametersheston(heston1d* hs,double correlation,double meanreversion,double longtermvariance,double volofvol)
{
    hs->correlation = correlation;
    hs->meanreversion = meanreversion;
    hs->longtermvariance = longtermvariance;
    hs->volofvol = volofvol;
}

void _set_ratesheston1d(heston1d* hs,rates interestrates, rates dividend)
{
	hs->_interestrates = interestrates;
	hs->_dividendrates = dividend;
}

void _initialize_explicitparams_heston(heston1d* hs)
{
	int nas = hs->numberofsteps;
	hs->_eps.Au = (double*)malloc(sizeof(double)*nas);
	hs->_eps.Bu = (double**)malloc(sizeof(double*)*nas);
	hs->_eps.Du = (double*)malloc(sizeof(double)*nas);
	hs->_eps.Eu = (double**)malloc(sizeof(double*)*nas);
	hs->_eps.Fu = (double**)malloc(sizeof(double*)*nas);
	int i=0;
	for(;i<nas;i++)
	{
		hs->_eps.Bu[i] = (double*)malloc(sizeof(double)*hs->_eps.dimension);
		hs->_eps.Eu[i] = (double*)malloc(sizeof(double)*hs->_eps.dimension);
		hs->_eps.Fu[i] = (double*)malloc(sizeof(double)*hs->_eps.dimension);
	}
}

void _set_model_parameters_heston1d(heston1d* hs,double timeslice,double expiry,double stepsize,int numberofspotsteps,double volstepsize,int numberofvolsteps)
{
    hs->dt = timeslice;
    hs->expiry = expiry;
    hs->stepsize = stepsize;
    hs->numberofsteps = numberofspotsteps;
    hs->volstepsize = volstepsize;
    hs->numberofvolsteps = numberofvolsteps;
    hs->nts = expiry/timeslice;
	_initialize_explicitparams_heston(hs);
}

void calc_timestepheston1d(heston1d *hs,results2d *_output,int i,int j,int k)
{
	int nas1 = hs->numberofsteps;
	int nas2 = hs->numberofvolsteps;
	double dt = hs->dt;
	double ds1 = hs->stepsize;
	double ds2 = hs->volstepsize;
	double variance = j*ds2;
	double sigma = pow(variance,0.5);
    double q = hs->volofvol*sigma;

	_output->prices[k*nas1*nas2+nas2*j+i] = hs->_eps.Bu[i][0]*_output->prices[k*nas1*nas2+nas2*j+i-1];
	_output->prices[k*nas1*nas2+nas2*j+i]+= hs->_eps.Bu[j][1]*_output->prices[k*nas1*nas2+nas2*(j-1)+i];
	_output->prices[k*nas1*nas2+nas2*j+i]+= _output->prices[(k-1)*nas1*nas2+nas2*j+i]*hs->_eps.Du[i];
	_output->prices[k*nas1*nas2+nas2*j+i]+= _output->prices[(k-1)*nas1*nas2+nas2*j+i-1]*hs->_eps.Eu[i][0];
	_output->prices[k*nas1*nas2+nas2*j+i]+= _output->prices[(k-1)*nas1*nas2+nas2*(j-1)+i]*hs->_eps.Eu[j][1];
	_output->prices[k*nas1*nas2+nas2*j+i]+= (_output->prices[(k-1)*nas1*nas2+nas2*j+i+1]-_output->prices[(k-1)*nas1*nas2+nas2*j+i])*hs->_eps.Fu[i][0];
	_output->prices[k*nas1*nas2+nas2*j+i]+= (_output->prices[(k-1)*nas1*nas2+nas2*(j+1)+i]-_output->prices[(k-1)*nas1*nas2+nas2*j+i])*hs->_eps.Fu[j][1];
	_output->prices[k*nas1*nas2+nas2*j+i]+= (dt*hs->correlation*(i*ds1)*q*sigma/(4.0*ds1*ds2))*(_output->prices[(k-1)*nas1*nas2+nas2*(j+1)+i+1]-_output->prices[(k-1)*nas1*nas2+nas2*(j+1)+i-1]-_output->prices[(k)*nas1*nas2+nas2*(j-1)+i+1]+_output->prices[(k)*nas1*nas2+nas2*(j-1)+i-1]);
	_output->prices[k*nas1*nas2+nas2*j+i] /= hs->_eps.Au[i];
}

void calculate_paramsheston1d(heston1d* hs,double intrate,double divrate,int j,int i)
{
	double dt = hs->dt;
    double ds1 = hs->stepsize;
    double ds2 = hs->volstepsize;
    double variance = j*ds2;
    double sigma = pow(variance,0.5);
    double p = hs->meanreversion*(hs->longtermvariance-variance);
    double q = hs->volofvol*sigma;

    //generic equation dsigma = pdt + qdX1
    if(j != -1)
	{
		hs->_eps.Bu[j][1] = 0.5*q*q*dt/(ds2*ds2);
		hs->_eps.Eu[j][1] = -p*dt/ds2;
		hs->_eps.Fu[j][1] = 0.5*q*q*dt/(ds2*ds2);
	}
	if(i != -1)
	{
		hs->_eps.Au[i] = 1 + 0.5*(j*ds2)*(i*ds1)*(i*ds1)*dt/(ds1*ds1)+0.5*q*q*dt/(ds2*ds2)+intrate*i*ds1*dt/(ds1)+p*dt/ds2;
		hs->_eps.Bu[i][0] = 0.5*(j*ds2)*(i*ds1)*(i*ds1)*dt/(ds1*ds1);
		hs->_eps.Eu[i][0] = -intrate*i*ds1*dt/ds1;
		hs->_eps.Fu[i][0] = 0.5*(j*ds2)*(i*ds1)*(i*ds1)*dt/(ds1*ds1);
		hs->_eps.Du[i] = 1 - intrate*dt;
	}
}

void apply_boundary_conditionsheston1d(heston1d* hs,results2d *_output,double intrate,int i,int j,int k)
{
	int nas1 = hs->numberofsteps;
	int nas2 = hs->numberofvolsteps;
    double dt = hs->dt;

	if(i==0 && k>0)
		_output->prices[k*nas1*nas2+nas2*j+i]=_output->prices[(k-1)*nas1*nas2+nas2*j+i] * (1-intrate*dt);

	if(i==(nas1-1) && k>0)
		_output->prices[k*nas1*nas2+nas2*j+i]=2*_output->prices[k*nas1*nas2+nas2*j+i-1] - _output->prices[k*nas1*nas2+nas2*j+i-2];

	if(j==0 && k>0)
		_output->prices[k*nas1*nas2+nas2*j+i]=_output->prices[(k-1)*nas1*nas2+nas2*j+i] * (1-intrate*dt);

	if(j==(nas2-1) && k>0)
		_output->prices[k*nas1*nas2+nas2*j+i]=2*_output->prices[k*nas1*nas2+nas2*(j-1)+i] - _output->prices[k*nas1*nas2+nas2*(j-2)+i];
}

results2d solve_experimentalheston1d(heston1d* hs,double increment)
{

    double dt = hs->dt;
    int i,j,k;
    double ds = hs->stepsize;
    int nts = hs->expiry/dt;
    hs->nts = nts;
    int nas1 = hs->numberofsteps;
	int nas2 = hs->numberofvolsteps;
    results2d _output;
    _init_results2d(&_output,nas1,nts);
	double divrate;
	double intrate;

    for(k=0;k<nts;k++)
    {
        //upstream - downstream to code (animesh)
        		divrate = hs->_dividendrates.get_rate_with_reftime(&(hs->_dividendrates),((nts-k)*dt));

        for(j=0;j<nas2;j++)
        {
              intrate = hs->_interestrates.get_rate_with_reftime(&(hs->_interestrates),((nts-k)*dt));

                for(i=0;i<nas1;i++)
                {
                    apply_boundary_conditionsheston1d(hs,&_output,intrate,i,j,k);
                    calculate_paramsheston1d(hs,intrate,divrate,j,i);

                    if(i!=0 && i!=(nas1-1) && (j!=0) && j!=(nas2-1) && k>0)
                    {
						calc_timestepheston1d(hs,&_output,i,j,k);
                    }

                   if((hs->apply_cashflow) != NULL)
                        _output.prices[k*nas1*nas2+nas2*j+i]+= hs->apply_cashflow(i*ds,k*dt);

                   if((hs->apply_coupon) != NULL)
                   {
                       if(hs->apply_coupon(i*ds,k*dt) != 0.0 && k != 0.0)
                            _output.prices[k*nas1*nas2+nas2*j+i]= hs->apply_coupon(i*ds,k*dt);
                   }


               }

          }
    }

return _output;
}


results2d _solveheston1d(heston1d* hs)
{
    int i,j,k;
    double inc = 0.0;
	results2d _output = solve_experimentalheston1d(hs,inc);
	hs->nts = (int)(hs->expiry / hs->dt);
	int nas1 = hs->numberofsteps;
	int nas2 = hs->numberofvolsteps;
    for(k=0;k<hs->nts;k++)
    {
        for(j=0;j<hs->numberofvolsteps-1;j++)
        {
            for(i=0;i<hs->numberofsteps-1;i++)
            {
                if(j>1)
                    _output.vega[k*nas1*nas2+nas2*j+i] = _output.prices[k*nas1*nas2+nas2*(j+1)+i] - _output.prices[k*nas1*nas2+nas2*(j-1)+i];
                if(i > 1)
                {
                    _output.delta[k*nas1*nas2+nas2*j+i] = (_output.prices[k*nas1*nas2+nas2*j+i+1]-_output.prices[k*nas1*nas2+nas2*j+i-1])/(2*hs->stepsize);
                    _output.gamma[k*nas1*nas2+nas2*j+i] = (_output.prices[k*nas1*nas2+nas2*j+i+1] + _output.prices[k*nas1*nas2+nas2*j+i-1] - 2*_output.prices[k*nas1*nas2+nas2*j+i])/(hs->stepsize*hs->stepsize);
                }
                if(k > 1)
                    _output.theta[k*nas1*nas2+nas2*j+i] = (_output.prices[(k-1)*nas1*nas2+nas2*j+i] - _output.prices[k*nas1*nas2+nas2*j+i])/(hs->dt);
            }
        }
    }
	return _output;
}


