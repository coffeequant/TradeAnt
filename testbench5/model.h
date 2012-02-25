#include <stdio.h>
#include <malloc.h>
#include <math.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_spline.h>

#include "result.h"
#include "boundary.h"
#include "volatility.h"
#include "rates.h"
#include "marketinstruments.h"


double interpolate(double *x,double *y,double xi);

  typedef struct _blackscholes1d
  {
    double stepsize;
    double dt;
    //similarity solution might create a problem, coz then you have an extra variable...and hence a different PDE!!!
    //how to modify the PDE???
    volsurface _blackscholesvol;
    rates _interestrates;
    rates _dividendrates;
    //interestcurve blackscholesintcurve;

    market_instruments* _m;
    int _mi_count;

    double expiry;
    int numberofsteps;
    int nts;
    //can't be more than 100
    int boundarycount;
    cashflows1d bc[100];
    results (*solve)(struct _blackscholes1d* bs);
    void (*set_vol_surface)(struct _blackscholes1d *bs,volsurface v);
    void (*set_rates)(struct _blackscholes1d* bs,rates interestrate,rates dividend);
    void (*add_cash_flows)(struct _blackscholes1d*,cashflows1d bc);
    double (*apply_cashflow)(double spot,double time,results*);
    void (*hedge_instruments)(struct _blackscholes1d*,double,double,results*);
  }blackscholes1d;
  
void initialize_blackscholes1d(blackscholes1d*);
void _set_vol_surface(blackscholes1d*,volsurface);
void _set_rates(blackscholes1d*,rates,rates);
void _add_cash_flows(blackscholes1d*,cashflows1d);
results solvebs(blackscholes1d*);
results solvebs_explicit(blackscholes1d*);
void _hedge_instruments(blackscholes1d*,double,double,results*);

