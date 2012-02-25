
#include <stdio.h>
#include <string.h>
#include <gsl/gsl_spline.h>
#include <gsl/gsl_errno.h>

#include "volatility.h"

#define LINELENGTH 1000
#define SEPARATOR ","
#define VOLCOOKIE "ANIVOL"

//data structure for storing volatility surface
//need a data structure for representing dates in C and how to add or subtract days to them...maybe a calendar structure
//problem is loading from CSV File then, how to keep converting two and fro between excel and C format....

void initialize_volsurface(volsurface* v)
{
  v->fetch_volatility_surface=_fetch_volatility_surface;
  v->get_vol_with_date = _get_vol_with_date;
  v->get_vol_with_reftime = _get_vol_with_reftime;
  v->set_size = _set_size;
  v->bump_bucket_with_date = _bump_bucket_with_date;
  v->bump_bucket_with_reftime = _bump_bucket_with_reftime;
  v->bump_surface = _bump_surface;
  v->set_volatility_surface = _set_volatility_surface;
}
                                                                                                                                                        


double interpolate(double *y,double *x,double xi,int arraysize)
{
  gsl_interp_accel *acc = gsl_interp_accel_alloc();
  gsl_spline *spline = gsl_spline_alloc(gsl_interp_cspline,arraysize);
  gsl_spline_init(spline,x,y,arraysize);
  double interpval =  gsl_spline_eval(spline,xi,acc);
  gsl_spline_free(spline);
  gsl_interp_accel_free(acc);
  return interpval;
}
  void _set_volatility_surface(volsurface* v,double* ustrikes,double* umaturities,double** uvolatility)
  {
    int i,j;
	for(i=0;i<v->strikecount;i++)
		v->strikes[i] = ustrikes[i];
	for(j=0;j<v->matcount;j++)
		v->maturities[j] = umaturities[j];
	for(i=0;i<v->matcount;i++)
		for(j=0;j<v->strikecount;j++)
			v->volatility[i][j] = uvolatility[i][j];
  }

void _set_size(volsurface *v,int strikesc,int maturitiesc)
{
  extern struct tm _globalref;
  v->strikecount = strikesc;
  v->matcount = maturitiesc;
    //initialize memory ...thank god....we are not using bullshit C++
  v->strikes = (double*)malloc(sizeof(double)*strikesc);
  v->maturities = (double*)malloc(sizeof(double)*maturitiesc);

  v->absolute_maturities = (qdate*)malloc(sizeof(qdate)*maturitiesc);
//init dates
  int i;
  for(i=0;i<maturitiesc;i++) {
	initqdate(&(v->absolute_maturities[i]));
	v->absolute_maturities[i].initrefdate(&v->absolute_maturities[i],_globalref.tm_year+1900,_globalref.tm_mon,_globalref.tm_mday);
  }
	
  v->volatility = (double**)malloc(sizeof(double*)*maturitiesc);
  
  for(i=0;i<maturitiesc;i++)
    v->volatility[i] = (double*)malloc(sizeof(double)*strikesc);
}
void _fetch_volatility_surface(volsurface *csvvol,char* filename)
{
  FILE *fp;
  if((fp = fopen(filename,"r"))==NULL)
    {
      printf("Can't open file %s\n",filename);
      exit(1);
    }
  else
    {
      //read it line by line
      char newline[LINELENGTH];
	fgets(newline,LINELENGTH,fp);
       if(strstr(newline,VOLCOOKIE)==NULL)
      {
        printf("Error in Vol Surface\n");
        exit(1);
      	}
      int i=0,j=0,k=0;
     while(i<csvvol->matcount-1)
	 {
	  fgets(newline,LINELENGTH,fp);
	  //split strikes it into comma separated values 
	  if(i==0)
	    {
	      	char* strike1;
	      	strike1=strtok(newline,SEPARATOR);
	
		for(k=1;k<csvvol->strikecount;k++)
	      		csvvol->strikes[k]=atof(strtok(NULL,SEPARATOR));
		csvvol->strikes[0]=atof(strike1);
          }
	  else
	  {
	      	char *date;
		date=strtok(newline,SEPARATOR);
	      for(k=0;k<csvvol->strikecount;k++) {
		csvvol->volatility[i][k] = atof(strtok(NULL,SEPARATOR));
		}
	      	csvvol->absolute_maturities[j++].initstringdate(&csvvol->absolute_maturities[j],date);
		csvvol->maturities[j-1] = csvvol->absolute_maturities[j-1].difference;
	     
	  }
	  //vol surface loaded
	  i++;
	}
      
    }
}

double _get_vol_with_reftime(volsurface* csvvol,double difference,double strike)
{
    //interpolation
    int i=0;
    for(;i<csvvol->matcount;i++)
      if(csvvol->absolute_maturities[i].difference >= difference) goto exitLoop;
  exitLoop:
    i = i - 1;i = i +1;

    double *v1array = (double*)malloc(sizeof(double)*(csvvol->strikecount));
    double *v2array = (double*)malloc(sizeof(double)*(csvvol->strikecount));
    int date1 = i;
    int date2 = i+1;
//	printf("%f %d",csvvol->strikes[0],date1);exit(0);
	
    for(i=0;i<csvvol->strikecount;i++) {
      v1array[i] = csvvol->volatility[date1][i];
      v2array[i] = csvvol->volatility[date2][i];
    }
	
if(strike < csvvol->strikes[0])
		strike = csvvol->strikes[0];
if(strike > csvvol->strikes[csvvol->strikecount-1])
		strike = csvvol->strikes[csvvol->strikecount-1];

	double v1 = interpolate(v1array,csvvol->strikes,strike,csvvol->strikecount);
    double v2 = interpolate(v2array,csvvol->strikes,strike,csvvol->strikecount);
    double t1 = csvvol->maturities[date1];
    double t2 = csvvol->maturities[date2];
	if(t1 == difference) return v1;
	if(t2 == difference) return v2;
      double rv = (v1*(difference-t1) + v2*(t2-difference))/(t2-t1);
    return rv;
}

double _get_vol_with_date(volsurface* csvvol,qdate date,double strike)
  {
    //interpolation
     int i=0;
    for(;i<csvvol->matcount;i++)
      if(csvvol->absolute_maturities[i].difference >= date.difference) goto exitLoop;
    
  exitLoop:
    i = i - 1;i = i +1;

    double *v1array = (double*)malloc(sizeof(double)*(csvvol->strikecount));
    double *v2array = (double*)malloc(sizeof(double)*(csvvol->strikecount));
    int date1 = i;
    int date2 = i+1;
    for(i=0;i<csvvol->strikecount;i++) {
      v1array[i] = csvvol->volatility[date1][i];
      v2array[i] = csvvol->volatility[date2][i];
    }
   
    double v1 = interpolate(v1array,csvvol->strikes,strike,csvvol->strikecount);
    double v2 = interpolate(v2array,csvvol->strikes,strike,csvvol->strikecount);
    double t1 = csvvol->maturities[date1];
    double t2 = csvvol->maturities[date2];
	exit(0);
    if(t1 == date.difference) return v1;
    if(t2 == date.difference) return v2;

    double rv = (v1*(date.difference-t1) + v2*(t2-date.difference))/(t2-t1);
    return rv; 
 }

double* _get_bucket(volsurface *csvvol, double date)
{

	return NULL;
}

void _bump_surface(volsurface *csvvol, double bumpamount)
{
 int i=0,j=0;
    for(;i<csvvol->matcount;i++)
       for(;j<csvvol->strikecount;j++) 
          csvvol->volatility[i][j]+=bumpamount;
}

void _bump_bucket_with_reftime(volsurface *csvvol,double difference, double bumpamount)
{
 int i=0;
    for(;i<csvvol->matcount;i++)
      if(csvvol->absolute_maturities[i].difference >= difference) goto exitLoop;
    
  exitLoop:
    i = i - 1;i = i +1;

    int date1 = i;
    for(i=0;i<csvvol->strikecount;i++) {
      csvvol->volatility[date1][i]=csvvol->volatility[date1][i]+bumpamount;
    }

}

void _bump_bucket_with_date(volsurface *csvvol,qdate date,double bumpamount)
{
    int i=0;
    for(;i<csvvol->matcount;i++)
      if(csvvol->absolute_maturities[i].difference >= date.difference) goto exitLoop;
    
  exitLoop:
    i = i - 1;i = i +1;

    int date1 = i;
    for(i=0;i<csvvol->strikecount;i++) {
      csvvol->volatility[date1][i]=csvvol->volatility[date1][i]+bumpamount;
    }
}

//extra stuff - for more exotic timings....

char* check_for_arbitrage()
{

  return NULL;
}



