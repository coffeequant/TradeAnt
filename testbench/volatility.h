#ifndef _dates_
#define _dates_
#include "dates.h"
#endif


typedef struct vol_surface
{
  //simplest crude data structure coz volatility surface can't be that large and we don't need fancy stuff - Animesh
  int strikecount;
  int matcount;
  double* strikes;
  double* maturities;
  qdate* absolute_maturities;
  double** volatility;

  //function pointers
  void (*fetch_volatility_surface)(struct vol_surface*,char*);
  double (*get_vol)(struct vol_surface*,qdate,double);
  void (*set_size)(struct vol_surface*,int,int);
} volsurface;


void _fetch_volatility_surface(volsurface*,char*);
double _get_vol(volsurface*,qdate,double);
void _set_size(volsurface*,int,int);



double initializevol(volsurface* v)
{
  v->fetch_volatility_surface=_fetch_volatility_surface;
  v->get_vol = _get_vol;
  v->set_size = _set_size;
}
