/*
 Copyright (C) 2012 Animesh Saxena

 This file is part of TradeAnt, a free-software/open-source library
 for traders and practical quantitative analysts

 TradeAnt is free software: you can redistribute it. Use it at your own risk
*/

#include <stdio.h>
#include <tradeant/blackscholes2d.h>
#include <math.h>

#include <gsl/gsl_errno.h>
#include <gsl/gsl_linalg.h>

//this file combines all the data and is the main pricing engine
//the non standard PDE would allow other than standard variables to have two extra terms....
//problem is how to decide the order of loops....
//first is always time...but depending on boundary conditions out of 3 which one's come first....


void _set_rates2d(blackscholes2d* bs,rates interestrates, rates dividends[])
{
	bs->_interestrates = interestrates;
	int p=0;
	for(;p<2;p++)
		bs->_dividendrates[p] = dividends[p];
}


void _initialize_explicitparams(blackscholes2d* bs)
{
	int nas = bs->numberofsteps;
	bs->_eps.Au = (double*)malloc(sizeof(double)*nas);
	bs->_eps.Bu = (double**)malloc(sizeof(double*)*nas);
	bs->_eps.Du = (double*)malloc(sizeof(double)*nas);
	bs->_eps.Eu = (double**)malloc(sizeof(double*)*nas);
	bs->_eps.Fu = (double**)malloc(sizeof(double*)*nas);
	int i=0;
	for(;i<nas;i++)
	{
		bs->_eps.Bu[i] = (double*)malloc(sizeof(double)*bs->_eps.dimension);
		bs->_eps.Eu[i] = (double*)malloc(sizeof(double)*bs->_eps.dimension);
		bs->_eps.Fu[i] = (double*)malloc(sizeof(double)*bs->_eps.dimension);
	}
}

void _set_model_parameters2d(struct _blackscholes2d* bs,double timeslice,double expiry,double stepsize,int numberofspotsteps,double correlation)
{
    bs->dt = timeslice;
    bs->expiry = expiry;
    bs->stepsize = stepsize;
    bs->numberofsteps = numberofspotsteps;
    bs->correlation = correlation;
	_initialize_explicitparams(bs);
}

void initialize_blackscholes2d(blackscholes2d *bs)
{
	  bs->solve = _solvebs2d;
	  bs->set_vol_surface = _set_vol_surface2d;
	  bs->set_rates = _set_rates2d;
	  bs->apply_cashflow = NULL;
      bs->set_model_parameters = _set_model_parameters2d;
	  bs->_eps.dimension = 2;
}

void _set_vol_surface2d(blackscholes2d *bs,volsurface v[])
{
	int p=0;
	for(;p<2;p++)
		bs->_blackscholesvol[p] = v[p];
}

void calc_timestep(blackscholes2d *bs,results2d *_output,int i,int j,int k,double *volatility)
{
	int nas = bs->numberofsteps;
    double dt = bs->dt;

	_output->prices[k*nas*nas+nas*j+i] = bs->_eps.Bu[i][0]*_output->prices[k*nas*nas+nas*j+i-1];
	_output->prices[k*nas*nas+nas*j+i]+= bs->_eps.Bu[j][1]*_output->prices[k*nas*nas+nas*(j-1)+i];
	_output->prices[k*nas*nas+nas*j+i]+= _output->prices[(k-1)*nas*nas+nas*j+i]*bs->_eps.Du[i];
	_output->prices[k*nas*nas+nas*j+i]+= _output->prices[(k-1)*nas*nas+nas*(j)+(i-1)]*bs->_eps.Eu[i][0];
	_output->prices[k*nas*nas+nas*j+i]+= _output->prices[(k-1)*nas*nas+nas*(j-1)+(i)]*bs->_eps.Eu[j][1];
	_output->prices[k*nas*nas+nas*j+i]+= (_output->prices[(k-1)*nas*nas+nas*(j)+i+1]-_output->prices[(k-1)*nas*nas+nas*(j)+i])*bs->_eps.Fu[i][0];
	_output->prices[k*nas*nas+nas*j+i]+= (_output->prices[(k-1)*nas*nas+nas*(j+1)+i]-_output->prices[(k-1)*nas*nas+nas*(j)+i])*bs->_eps.Fu[j][1];
	_output->prices[k*nas*nas+nas*j+i]+= (dt*i*j/(4.0))*(_output->prices[(k-1)*nas*nas+(j+1)*nas+i+1]-_output->prices[(k-1)*nas*nas+(j+1)*nas+i-1]-_output->prices[(k)*nas*nas+(j-1)*nas+i+1]+_output->prices[(k)*nas*nas+(j-1)*nas+i-1])*bs->correlation*volatility[0]*volatility[1];
	_output->prices[k*nas*nas+nas*j+i] /= bs->_eps.Au[i];
}

void calculate_params(blackscholes2d* bs,double intrate,double* volatility,double* divrate,int j,int i)
{
	double dt = bs->dt;
    double ds = bs->stepsize;
    if(j != -1)
	{
		bs->_eps.Bu[j][1] = 0.5*volatility[1]*volatility[1]*j*ds*j*ds*dt/(ds*ds);
		bs->_eps.Eu[j][1] = -(intrate-divrate[1])*j*ds*dt/ds;
		bs->_eps.Fu[j][1] = 0.5*volatility[1]*volatility[1]*j*ds*j*ds*dt/(ds*ds);
	}
	if(i != -1)
	{
		bs->_eps.Au[i] = 1 + 0.5*volatility[1]*volatility[1]*j*ds*j*ds*dt/(ds*ds)+ 0.5*volatility[0]*volatility[0]*i*ds*i*ds*dt/(ds*ds) - intrate*(i*ds)*dt/ds  - intrate*(j*ds)*dt/ds;
		bs->_eps.Bu[i][0] = 0.5*volatility[0]*volatility[0]*i*ds*i*ds*dt/(ds*ds);
		bs->_eps.Eu[i][0] = -(intrate-divrate[0])*i*ds*dt/ds;
		bs->_eps.Fu[i][0] = 0.5*volatility[0]*volatility[0]*i*ds*i*ds*dt/(ds*ds);
		bs->_eps.Du[i] = 1 - intrate*dt;
	}
}

void apply_boundary_conditions2d(blackscholes2d* bs,results2d *_output,double intrate,int i,int j,int k)
{
	int nas = bs->numberofsteps;
    double dt = bs->dt;

	if(i==0 && k>0)
		_output->prices[nas*nas*k+nas*j+i]=_output->prices[nas*nas*(k-1)+nas*j+i] * (1-intrate*dt);

	if(i==(nas-1) && k>0)
		_output->prices[nas*nas*k+nas*j+i]=2*_output->prices[nas*nas*k+nas*j+i-1] - _output->prices[nas*nas*k+nas*j+i-2];

	if(j==0 && k>0)
		_output->prices[nas*nas*k+nas*j+i]=_output->prices[nas*nas*(k-1)+nas*(j)+i] * (1-intrate*dt);

	if(j==(nas-1) && k>0)
		_output->prices[nas*nas*k+nas*j+i]=2*_output->prices[nas*nas*k+nas*(j-1)+i] - _output->prices[nas*nas*k+nas*(j-2)+i];

}

results2d solve_experimental2d(blackscholes2d* bs,double increment)
{

    double dt = bs->dt;
    int i,j,k;
    double ds = bs->stepsize;
    int nts = bs->expiry/dt;
    bs->nts = nts;
    int nas = bs->numberofsteps;
    results2d _output;
    _init_results2d(&_output,nas,nts);
	double volatility[2];
	double divrate[2];
	double intrate;
	double tmp[2];
    int p = 0;

    for(k=0;k<nts;k++)
    {
        //upstream - downstream to code (animesh)
        for(p=0;p<2;p++)
				divrate[p] = bs->_dividendrates[p].get_rate_with_reftime(&(bs->_dividendrates[p]),((nts-k)*dt));

        for(j=0;j<nas;j++)
        {

            	volatility[0] =bs->_blackscholesvol[p].get_vol_with_reftime(&(bs->_blackscholesvol[p]),((nts-k)*dt),(i*ds))+increment;
				volatility[1] =bs->_blackscholesvol[p].get_vol_with_reftime(&(bs->_blackscholesvol[p]),((nts-k)*dt),(j*ds))+increment;

                intrate = bs->_interestrates.get_rate_with_reftime(&(bs->_interestrates),((nts-k)*dt));


                for(i=0;i<nas;i++)
                {
                    apply_boundary_conditions2d(bs,&_output,intrate,i,j,k);
                    calculate_params(bs,intrate,volatility,divrate,j,i);

                    if(i!=0 && i!=(nas-1) && (j!=0) && j!=(nas-1) && k>0)
                    {
						calc_timestep(bs,&_output,i,j,k,volatility);
                    }

                   tmp[1] = j*ds;tmp[0] = i*ds;

                   if((bs->apply_cashflow) != NULL)
                        _output.prices[k*nas*nas+nas*j+i]+= bs->apply_cashflow(tmp,k*dt);

                    if((bs->apply_coupon) != NULL)
                      _output.prices[k*nas*nas+nas*j+i] = bs->apply_coupon(tmp,k*dt);
               }

          }
    }

return _output;
}


results2d _solvebs2d(blackscholes2d* bs)
{
    int i,j,k;
    double inc = 0.0;
	results2d _output = solve_experimental2d(bs,inc);
	results2d _output_2 = solve_experimental2d(bs,inc+0.01);
    bs->nts = (int)(bs->expiry / bs->dt);
	int nas = bs->numberofsteps;
    for(k=0;k<bs->nts;k++)
    {
        for(j=0;j<bs->numberofsteps-1;j++)
        {
            for(i=0;i<bs->numberofsteps-1;i++)
            {
                _output.vega[k*nas*nas+j*nas+i] = _output.prices[k*nas*nas+j*nas+i] - _output_2.prices[k*nas*nas+j*nas+i];
                if(i > 1)
                {
                    _output.delta[k*nas*nas+j*nas+i] = _output.prices[k*nas*nas+j*nas+i+1]-_output.prices[k*nas*nas+j*nas+i-1]/(2*bs->stepsize);
                    _output.gamma[k*nas*nas+j*nas+i] = (_output.prices[k*nas*nas+j*nas+i+1] + _output.prices[k*nas*nas+j*nas+i-1] - 2*_output.prices[k*nas*nas+j*nas+i])/(bs->stepsize*bs->stepsize);
                }
                if(k > 1)
                    _output.theta[k*nas*nas+j*nas+i] = (_output.prices[(k-1)*nas*nas+j*nas+i] - _output.prices[k*nas*nas+j*nas+i])/(bs->dt);
            }
        }
    }
	return _output;
}


