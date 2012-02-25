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
  double (*get_vol_with_date)(struct vol_surface*,qdate,double);
  double (*get_vol_with_reftime)(struct vol_surface*,double,double);
  void (*set_size)(struct vol_surface*,int,int);
  double (*bump_bucket_with_reftime)(struct vol_surface*,double,double);
  double (*bump_bucket_with_date)(struct vol_surface*,qdate,double);

} volsurface;


void _fetch_volatility_surface(volsurface*,char*);
double _get_vol_with_date(volsurface*,qdate,double);
double _get_vol_with_reftime(volsurface*,double,double);
void _set_size(volsurface*,int,int);
void initialize_volsurface(volsurface*);

double _bump_bucket_with_reftime(volsurface *csvvol,double difference, double bumpamount);
double _bump_bucket_with_date(volsurface *csvvol,qdate date,double bumpamount);
    
