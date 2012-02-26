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


void _set_rates(blackscholes2d* bs,rates interestrates[], rates dividends[])
{
	bs->_interestrates[0] = interestrates[0];
	bs->_interestrates[1] = interestrates[1];

	bs->_dividendrates[0] = dividends[0];
	bs->_dividendrates[1] = dividends[1];
}


void initialize_blackscholes2d(blackscholes2d *bs)
{
	  bs->solve = solvebs2d;
	  bs->set_vol_surface = _set_vol_surface;
	  bs->set_rates = _set_rates;
	  bs->apply_cashflow = NULL;
	  bs->nts = (int)(bs->expiry / bs->dt);
}

void _set_vol_surface(blackscholes2d *bs,volsurface v[])
{
	bs->_blackscholesvol[0] = v[0];
	bs->_blackscholesvol[1] = v[1];
}


void _init_results2d(results *_output,int numberofsteps,int nts)
{
   int i;
        _output->prices[i] = (double*)malloc(sizeof(double)*numberofsteps*numberofsteps*nts);
        _output->delta[i] = (double*)malloc(sizeof(double)*numberofsteps*numberofsteps*nts);
        _output->theta[i] = (double*)malloc(sizeof(double)*numberofsteps*numberofsteps*nts);
        _output->gamma[i] = (double*)malloc(sizeof(double)*numberofsteps*numberofsteps*nts);
        _output->vega[i] = (double*)malloc(sizeof(double)*numberofsteps*numberofsteps*nts);
}


results solve_experimental2d(blackscholes2d* bs,double increment)
{
    double dt = bs->dt;
    int i,j,k;
    double ds = bs->stepsize;
    int nts = bs->expiry/dt;
    int nas = bs->numberofsteps;
    results _output;
    _init_results2d(&_output,bs->numberofsteps,bs->nts);
    double *Au = (double*)malloc(sizeof(double)*nas);
    double *Bu = (double*)malloc(sizeof(double)*nas);
    double *Du = (double*)malloc(sizeof(double)*nas);
    double *Eu = (double*)malloc(sizeof(double)*nas);
    double *Fu = (double*)malloc(sizeof(double)*nas);

    double *Au2 = (double*)malloc(sizeof(double)*nas);
    double *Bu2 = (double*)malloc(sizeof(double)*nas);
    double *Du2 = (double*)malloc(sizeof(double)*nas);
    double *Eu2 = (double*)malloc(sizeof(double)*nas);
    double *Fu2 = (double*)malloc(sizeof(double)*nas);

    //double *Cross = (double*)malloc(sizeof(double)*nas*nas);
    //200*200*i+200*j+k
    //1.............199
    //200...........399
    //200*200..............
    //

    //0

    double volatility,intrate,divrate;
    for(k=0;k<nts;k++)
    {

        //upstream
        for(j=0;j<nas;j++)
        {
                Au2[j] = 1 - intrate*(j*ds)*dt/ds+0.5*volatility*volatility*j*ds*j*ds*dt/(ds*ds);
                Bu2[j] = 0.5*volatility*volatility*j*ds*j*ds*dt/(ds*ds);
                Eu2[j] = -intrate*j*ds*dt/ds;
                Fu2[j] = 0.5*volatility*volatility*j*ds*j*ds*dt/(ds*ds);
                Du2[j] = 1 - intrate*dt;

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

                if(i!=0 && i!=(nas-1) && k>0)
                {
                    _output.prices[i][j] = (Bu[i]*_output.prices[i-1][j]+_output.prices[i][j-1]*Du[i]+_output.prices[i-1][j-1]*Eu[i]+(_output.prices[i+1][j-1]-_output.prices[i][j-1])*Fu[i])/Au[i];
                }

                if(i==0 && k>0)
                {
                    _output.prices[nas*nas*k+nas*j+i]=_output.prices[nas*nas*k+nas*j+i-1]*(1-intrate*dt);
                }

                if(i==(nas-1))
                {
                    _output.prices[i][j]=2*_output.prices[i-1][j] - _output.prices[i-2][j];
                }

                if((bs->hedge_instruments) != NULL)
                    _hedge_instruments(bs,i*ds,j*dt,&_output);

                if((bs->apply_cashflow) != NULL)
                    _output.prices[i][j] += bs->apply_cashflow(i*ds,j*dt,&_output);
            }
     }


return _output;
}


results solvebs2d(blackscholes2d* bs)
{
    double inc = 0.0;
	results _output = solve_experimental2d(bs,inc);
	results _output_2 = solve_experimental2d(bs,inc+0.01);
}


