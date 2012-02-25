#include <stdio.h>

#include <math.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_spline.h>
#include "result.h"
#ifndef _boundary_
#define _boundary_
#include "boundary.h"
#endif

#ifndef _volatility_
#define _volatility_
#include "volatility.h"
#endif

double interpolate(double *x,double *y,double xi);

  typedef struct _blackscholes1d
  {
    double stepsize;
    double dt;
    //similarity solution might create a problem, coz then you have an extra variable...and hence a different PDE!!!
    //how to modify the PDE???
    volsurface _blackscholesvol;
    //interestcurve blackscholesintcurve;

    int numberofsteps;
    int nts;
    //can't be more than 100
    boundaryconditions1D bc;
    results (*solve)(struct _blackscholes1d* bs);
    void (*set_vol_surface)(struct _blackscholes1d *bs,volsurface v);
    void (*apply_boundary_conditions)(struct _blackscholes1d*,boundaryconditions1D bc);

  }blackscholes1d;
  
void initialize_blackscholes1d(blackscholes1d *bs);
void _set_vol_surface(blackscholes1d *bs,volsurface v);
void _apply_boundary_conditions(blackscholes1d*,boundaryconditions1D);
results solvebs(blackscholes1d* bs);

