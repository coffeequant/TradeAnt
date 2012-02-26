
/*
 Copyright (C) 2012 Animesh Saxena

 This file is part of TradeAnt, a free-software/open-source library
 for traders and practical quantitative analysts

 TradeAnt is free software: you can redistribute it. Use it at your own risk
*/

#ifndef _rates_
#define _rates_
#include <stdio.h>
#include <stdlib.h>
#include <tradeant/dates.h>

//need interest rate and dividend curves both
typedef struct interestcurve
{
  double* maturities;
  double* rate;
  qdate* absolute_maturities;
  int matcount;
  double constantrate;

  void (*set_size)(struct interestcurve*,int);
  void (*fetch_rates)(struct interestcurve*,char*);
  double (*get_rate_with_date)(struct interestcurve*,qdate);
  double (*get_rate_with_reftime)(struct interestcurve*,double);
  void (*bump_rates_with_reftime)(struct interestcurve*,double,double);
  void (*bump_rates_with_date)(struct interestcurve*,qdate,double);
  void (*bump_rates)(struct interestcurve*,double);
  void (*set_rates_with_reftime)(struct interestcurve*,double*,double*);
  void (*set_rates_with_date)(struct interestcurve*,qdate*,double*);
}rates;

void _set_rates_with_reftime(rates*,double*,double*);
void _set_rates_with_date(rates*,qdate*,double*);

void _fetch_rates(rates*,char*);
double _get_rate_with_date(rates*,qdate);
double _get_rate_with_reftime(rates*,double);

void initialize_rates(rates*);
void _bump_rates_with_reftime(rates*,double,double);
void _bump_rates_with_date(rates*,qdate,double);
void _bump_rates(rates*,double);
void _set_rates_size(rates*,int);
//how to incorporat discounting...the r(t) term in the equation....
//need interpolation libraries...
//flat or cubic or quadratic??
#endif
