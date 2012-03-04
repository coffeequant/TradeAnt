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

	bs->_dividendrates[0] = dividends[0];
	bs->_dividendrates[1] = dividends[1];
}

void _set_model_parameters_2d(struct _blackscholes2d* bs,double timeslice,double expiry,double stepsize,int numberofspotsteps)
{
    bs->dt = timeslice;
    bs->expiry = expiry;
    bs->stepsize = stepsize;
    bs->numberofsteps = numberofspotsteps;
}

void initialize_blackscholes2d(blackscholes2d *bs)
{
	  bs->solve = _solvebs2d;
	  bs->set_vol_surface = _set_vol_surface2d;
	  bs->set_rates = _set_rates2d;
	  bs->apply_cashflow = NULL;
      bs->set_model_parameters = _set_model_parameters2d;
}

void _set_vol_surface2d(blackscholes2d *bs,volsurface v[])
{
	bs->_blackscholesvol[0] = v[0];
	bs->_blackscholesvol[1] = v[1];
}


results2d solve_experimental2d(blackscholes2d* bs,double increment)
{

    double dt = bs->dt;
    int i,j,k;
    double ds = bs->stepsize;
    int nts = bs->expiry/dt;
    int nas = bs->numberofsteps;
    results2d _output;
    _init_results2d(&_output,bs->numberofsteps,bs->nts);
    double *Au = (double*)malloc(sizeof(double)*nas);
    double *Bu = (double*)malloc(sizeof(double)*nas);
    double *Du = (double*)malloc(sizeof(double)*nas);
    double *Eu = (double*)malloc(sizeof(double)*nas);
    double *Fu = (double*)malloc(sizeof(double)*nas);

    double *Bu1 = (double*)malloc(sizeof(double)*nas);
    double *Eu1 = (double*)malloc(sizeof(double)*nas);
    double *Fu1 = (double*)malloc(sizeof(double)*nas);

    double volatility1,volatility2,intrate,divrate1,divrate2;
    double tmp[2];

    for(k=0;k<nts;k++)
    {

        //upstream
        for(j=0;j<nas;j++)
        {
                     intrate = bs->_interestrates.get_rate_with_reftime(&(bs->_interestrates),((nts-j)*dt));
                     divrate1 = bs->_dividendrates[0].get_rate_with_reftime(&(bs->_dividendrates[0]),((nts-j)*dt));
                     divrate2 = bs->_dividendrates[1].get_rate_with_reftime(&(bs->_dividendrates[1]),((nts-j)*dt));
                     volatility1 = bs->_blackscholesvol[0].get_vol_with_reftime(&(bs->_blackscholesvol[0]),((nts-j)*dt),(i*ds))+increment;
                     volatility2 = bs->_blackscholesvol[1].get_vol_with_reftime(&(bs->_blackscholesvol[1]),((nts-j)*dt),(i*ds))+increment;

            Bu1[j] = 0.5*volatility1*volatility1*j*ds*j*ds*dt/(ds*ds);
            Eu1[j] = -(intrate-divrate1)*j*ds*dt/ds;
            Fu1[j] = 0.5*volatility1*volatility1*j*ds*j*ds*dt/(ds*ds);

                if(j==0 && k>0)
                {
                    _output.prices[nas*nas*k+nas*j+i]=_output.prices[nas*nas*(k-1)+nas*(j)+i] * (1-intrate*dt);
                }

                if(j==(nas-1) && k>0)
                {
                    _output.prices[nas*nas*k+nas*j+i]=2*_output.prices[nas*nas*k+nas*(j-1)+i] - _output.prices[nas*nas*k+nas*(j-2)+i];
                }


                for(i=0;i<nas;i++)
                {
                    Au[i] = 1 - (intrate-divrate2)*(i*ds)*dt/ds+0.5*volatility2*volatility2*i*ds*i*ds*dt/(ds*ds)-(intrate-divrate1)*(j*ds)*dt/ds+0.5*volatility1*volatility1*j*ds*j*ds*dt/(ds*ds);
                    Bu[i] = 0.5*volatility2*volatility2*i*ds*i*ds*dt/(ds*ds);
                    Eu[i] = -(intrate-divrate2)*i*ds*dt/ds;
                    Fu[i] = 0.5*volatility2*volatility2*i*ds*i*ds*dt/(ds*ds);
                    Du[i] = 1 - intrate*dt;

                    if(i==0 && k>0)
                    {
                        _output.prices[nas*nas*k+nas*j+i]=_output.prices[nas*nas*(k-1)+nas*j+i] * (1-intrate*dt);
                    }

                    if(i==(nas-1) && k>0)
                    {
                        _output.prices[nas*nas*k+nas*j+i]=2*_output.prices[nas*nas*k+nas*j+i-1] - _output.prices[nas*nas*k+nas*j+i-2];
                    }

                    if(i!=0 && i!=(nas-1) && (j!=0) && j!=(nas-1) && k>0)
                    {
                        _output.prices[k*nas*nas+nas*j+i] = (Bu[i]*_output.prices[k*nas*nas+nas*j+i-1]+Bu1[j]*_output.prices[k*nas*nas+nas*(j-1)+i]+_output.prices[(k-1)*nas*nas+nas*j+i]*Du[i]+_output.prices[(k-1)*nas*nas+nas*(j)+(i-1)]*Eu[i] + _output.prices[(k-1)*nas*nas+nas*(j-1)+(i)]*Eu1[j]+(_output.prices[(k-1)*nas*nas+nas*(j)+i+1]-_output.prices[(k-1)*nas*nas+nas*(j)+i])*Fu[i]+(_output.prices[(k-1)*nas*nas+nas*(j+1)+i]-_output.prices[(k-1)*nas*nas+nas*(j)+i])*Fu1[j] -(dt*i*j/(4))*(_output.prices[(k-1)*nas*nas+(j+1)*nas+i+1]-_output.prices[(k-1)*nas*nas+(j+1)*nas+i-1]-_output.prices[(k)*nas*nas+(j-1)*nas+i+1]+_output.prices[(k)*nas*nas+(j-1)*nas+i-1])*bs->correlation*volatility1*volatility2);
                        _output.prices[k*nas*nas+nas*j+i] /= Au[i];
                    }

                    tmp[0] = j*ds;
                    tmp[1] = i*ds;

                    if((bs->apply_cashflow) != NULL)
                    {
                        _output.prices[k*nas*nas+nas*j+i] += bs->apply_cashflow(tmp,k*dt);
                    }

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
	int nts = bs->nts;
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


