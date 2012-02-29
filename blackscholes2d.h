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

  typedef struct _blackscholes2d
  {
    double stepsize;
    double dt;
    volsurface _blackscholesvol[2];
    rates _interestrates;
    rates _dividendrates[2];
    double correlation;
    //interestcurve blackscholesintcurve;

    market_instruments* _m[3];
    int _mi_count;

    double expiry;
    int numberofsteps;
    int nts;
    //can't be more than 100
    results2d (*solve)(struct _blackscholes2d* bs);
    void (*set_vol_surface)(struct _blackscholes2d *bs,volsurface v[]);
    void (*set_rates)(struct _blackscholes2d* bs,rates interestrate,rates dividend[]);
    double (*apply_cashflow)(double spot[],double time);
}blackscholes2d;

void initialize_blackscholes2d(blackscholes2d*);
void _set_vol_surface2d(blackscholes2d*,volsurface[]);
void _set_rates2d(blackscholes2d*,rates,rates[]);
results2d _solvebs2d(blackscholes2d* bs);
void _hedge_instruments(blackscholes2d*,double,double,results2d*);


