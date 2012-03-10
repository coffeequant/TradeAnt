/*
 Copyright (C) 2012 Animesh Saxena

 This file is part of TradeAnt, a free-software/open-source library
 for traders and practical quantitative analysts

 TradeAnt is free software: you can redistribute it. Use it at your own risk
*/

#include <stdio.h>
#include <malloc.h>
#include <math.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_spline.h>

#include <tradeant/result.h>
#include <tradeant/volatility.h>
#include <tradeant/rates.h>
#include <tradeant/marketinstruments.h>


double interpolate(double *x,double *y,double xi);

typedef struct _explicitparamsheston
{
	int dimension;
	double *Au;
    double **Bu;
    double *Du;
    double **Eu;
    double **Fu;

}explicitparamsheston;


  typedef struct _heston1d
  {
    double stepsize;
    double dt;

    //cool no vol surface, just plain parameters
    rates _interestrates;
    rates _dividendrates;

	explicitparamsheston _eps;

    double expiry,volstepsize;
    int numberofsteps,numberofvolsteps;
    int nts;

    //heston specific params..
    double correlation,meanreversion,longtermvariance,volofvol;

   results2d (*solve)(struct _heston1d* hs);
   void (*set_model_parameters)(struct _heston1d *hs,double timeslice,double expiry,double stepsize,int numberofspotsteps,double volstepsize,int numberofvolsteps);
   void (*set_calibration_parameters)(struct _heston1d* hs,double correlation,double meanreversion,double longtermvariance,double volofvol);
   void (*set_rates)(struct _heston1d* hs,rates interestrate,rates dividend);
   double (*apply_cashflow)(double spot,double time);
   double (*apply_coupon)(double spot,double time);
}heston1d;


void _initialize_explicitparams_heston(heston1d* hs);
void initialize_heston1d(heston1d* hs);
void _set_ratesheston1d(heston1d*,rates,rates);
results2d _solveheston1d(heston1d* hs);
void _set_model_parameters_heston1d(heston1d*,double timeslice,double expiry,double stepsize,int numberofspotsteps,double volstepsize,int numberofvolsteps);
void _set_calibration_parametersheston(heston1d* hs,double correlation,double meanreversion,double longtermvariance,double volofvol);

