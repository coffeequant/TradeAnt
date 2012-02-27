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


void _set_rates2d(blackscholes2d* bs,rates interestrates[], rates dividends[])
{
	bs->_interestrates[0] = interestrates[0];
	bs->_interestrates[1] = interestrates[1];

	bs->_dividendrates[0] = dividends[0];
	bs->_dividendrates[1] = dividends[1];
}


void initialize_blackscholes2d(blackscholes2d *bs)
{
	  bs->solve = _solvebs2d;
	  bs->set_vol_surface = _set_vol_surface2d;
	  bs->set_rates = _set_rates2d;
	  bs->apply_cashflow = NULL;
	  bs->nts = (int)(bs->expiry / bs->dt);
}

void _set_vol_surface2d(blackscholes2d *bs,volsurface v[])
{
	bs->_blackscholesvol[0] = v[0];
	bs->_blackscholesvol[1] = v[1];
}


void _init_results2d(results2d* _output,int numberofsteps,int nts)
{
   int i;
        _output->prices = (double*)malloc(sizeof(double)*numberofsteps*numberofsteps*nts);
        _output->delta= (double*)malloc(sizeof(double)*numberofsteps*numberofsteps*nts);
        _output->theta = (double*)malloc(sizeof(double)*numberofsteps*numberofsteps*nts);
        _output->gamma = (double*)malloc(sizeof(double)*numberofsteps*numberofsteps*nts);
        _output->vega = (double*)malloc(sizeof(double)*numberofsteps*numberofsteps*nts);
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

    double volatility,intrate,divrate;
    double tmp[2];
    for(k=0;k<nts;k++)
    {

        //upstream
        for(j=0;j<nas;j++)
        {
                for(i=0;i<nas;i++)
                {
                    intrate = 0.05;//bs->_interestrates.get_rate_with_reftime(&(bs->_interestrates),((nts-j)*dt));
                    divrate = 0;//bs->_dividendrates.get_rate_with_reftime(&(bs->_dividendrates),((nts-j)*dt));
                    volatility = 0.20;//bs->_blackscholesvol.get_vol_with_reftime(&(bs->_blackscholesvol),((nts-j)*dt),(i*ds))+increment;

                    Au[i] = 1 - intrate*(i*ds)*dt/ds+0.5*volatility*volatility*i*ds*i*ds*dt/(ds*ds);
                    Bu[i] = 0.5*volatility*volatility*i*ds*i*ds*dt/(ds*ds);
                    Eu[i] = -intrate*i*ds*dt/ds;
                    Fu[i] = 0.5*volatility*volatility*i*ds*i*ds*dt/(ds*ds);
                    Du[i] = 1 - intrate*dt;

                    if(i!=0 && i!=(nas-1) && (j!=0) && j!=(nas-1) && k>0)
                    {
                        //_output.prices[i][j] = (Bu[i]*_output.prices[i-1][j]+_output.prices[i][j-1]*Du[i]+_output.prices[i-1][j-1]*Eu[i]+(_output.prices[i+1][j-1]-_output.prices[i][j-1])*Fu[i])/Au[i];
                        _output.prices[k*nas*nas+nas*j+i]=(Bu[i]*_output.prices[k*nas*nas+nas*j+i-1]+_output.prices[(k-1)*nas*nas+nas*j+i]*Du[i]+_output.prices[(k-1)*nas*nas+nas*(j)+(i-1)]*Eu[i] + (_output.prices[(k-1)*nas*nas+nas*(j)+i+1]-_output.prices[(k-1)*nas*nas+nas*(j)+i])*Fu[i])/Au[i];
                    }

                    if(i==0 && k>0)
                    {
                        _output.prices[nas*nas*k+nas*j+i]=_output.prices[nas*nas*(k-1)+nas*j+i] * (1-intrate*dt);
                    }

                    if(i==(nas-1))
                    {
                        _output.prices[nas*nas*k+nas*j+i]=2*_output.prices[nas*nas*k+nas*j+i-1] - _output.prices[nas*nas*k+nas*j+i-2];
                    }

                    tmp[0] = i*ds;
                    tmp[1] = j*ds;

                    if((bs->apply_cashflow) != NULL)
                    {
                        double cc = bs->apply_cashflow(tmp,k*dt);

                        _output.prices[k*nas*nas+nas*j+i] += cc;
                    //    if(i>100 )
                      //      printf("DD:%.2f",_output.prices[i]);
                    }

                }
                if(j==0 && k>0)
                {
                    _output.prices[nas*nas*k+nas*j+i]=_output.prices[nas*nas*(k-1)+nas*(j)+i] * (1-intrate*dt);
                }

                if(j==(nas-1))
                {
                    _output.prices[nas*nas*k+nas*j+i]=2*_output.prices[nas*nas*k+nas*(j-1)+i] - _output.prices[nas*nas*k+nas*(j-2)+i];
                }

          }
    }

return _output;
}


results2d _solvebs2d(blackscholes2d* bs)
{
    double inc = 0.0;
	results2d _output = solve_experimental2d(bs,inc);
//	results2d _output_2 = solve_experimental2d(bs,inc+0.01);

	return _output;
}


