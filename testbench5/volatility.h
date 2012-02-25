
#ifndef _volatility_
#define _volatility_

#include "dates.h"

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
  void (*bump_bucket_with_reftime)(struct vol_surface*,double,double);
  void (*bump_bucket_with_date)(struct vol_surface*,qdate,double);
  void (*bump_surface)(struct vol_surface*,double);
  void (*set_volatility_surface)(struct vol_surface*,double*,double*,double**);

} volsurface;


void _fetch_volatility_surface(volsurface*,char*);
double _get_vol_with_date(volsurface*,qdate,double);
double _get_vol_with_reftime(volsurface*,double,double);
void _set_size(volsurface*,int,int);
void initialize_volsurface(volsurface*);

void _bump_bucket_with_reftime(volsurface *csvvol,double difference, double bumpamount);
void _bump_bucket_with_date(volsurface *csvvol,qdate date,double bumpamount);
void _bump_surface(volsurface*, double);    
void _set_volatility_surface(volsurface*,double*,double*,double**);


#endif
