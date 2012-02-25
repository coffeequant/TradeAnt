
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
  void set_volatility_surface(double* ustrikes,double* umaturities,double** uvolatility)
  {
    

  }

void _set_size(volsurface *v,int strikesc,int maturitiesc)
{
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
	v->absolute_maturities[i].initrefdate(&v->absolute_maturities[i],2012,1,1);
  }
	
  v->volatility = (double**)malloc(sizeof(double*)*maturitiesc);
  
  for(i=0;i<maturitiesc;i++)
    v->volatility[i] = (double*)malloc(sizeof(double)*strikesc);
}
void _fetch_volatility_surface(volsurface *csvvol,char* filename)
{


//  volsurface csvvol;
 // initializevol(&csvvol);

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
		//strtok(NULL,SEPARATOR);
	
		for(k=1;k<csvvol->strikecount;k++)
	      		csvvol->strikes[k]=atof(strtok(NULL,SEPARATOR));
		csvvol->strikes[0]=atof(strike1);
 	      //csvvol->absolute_maturities[j++].initstringdate(&csvvol->absolute_maturities[j],date);
	      //csvvol->maturities[j-1] = csvvol->absolute_maturities[j-1].difference;
		//printf("%s",date);     
          }
	  else
	  {
	      	char *date;
		date=strtok(newline,SEPARATOR);
	//	printf("%s",strtok(NULL,SEPARATOR));
		//exit(0);
		//printf("%d",csvvol->strikecount);
	      for(k=0;k<csvvol->strikecount;k++) {
		csvvol->volatility[i][k] = atof(strtok(NULL,SEPARATOR));
		//if(k==19) exit(0);
		}
		//exit(0);
	      	csvvol->absolute_maturities[j++].initstringdate(&csvvol->absolute_maturities[j],date);
		csvvol->maturities[j-1] = csvvol->absolute_maturities[j-1].difference;
	     
	  }
	  //vol surface loaded
	//return;
	  i++;
	}
      
    }
 // return csvvol;
}

double _get_vol(volsurface* csvvol,qdate date,double strike)
  {
    //interpolation
    int i=0;
    for(;i<csvvol->matcount;i++)
      if(csvvol->absolute_maturities[i].difference >= date.difference) goto exitLoop;
    
  exitLoop:
//    if(csvvol->absolute_maturities[i].difference = date.difference) return csvvol->volatility[i][  
    i = i - 1;i = i +1;
//plain bullshut to conguse the goto statement

    double *v1array = (double*)malloc(sizeof(double)*(csvvol->strikecount));
    double *v2array = (double*)malloc(sizeof(double)*(csvvol->strikecount));
    int date1 = i;
    int date2 = i+1;
    for(i=0;i<csvvol->strikecount;i++) {
      v1array[i] = csvvol->volatility[date1][i];
      v2array[i] = csvvol->volatility[date2][i];
    }
    //now these are the wholesome buckets...
    //printf("%f",csvvol->strikes[0]);exit(0);
	double v1 = interpolate(v1array,csvvol->strikes,strike,csvvol->strikecount);
	//printf("%f",v1);exit(0);
    double v2 = interpolate(v2array,csvvol->strikes,strike,csvvol->strikecount);
//	printf("%f",csvvol->maturities[date2]);exit(0);
      //get t1 and t2 - two nearest time frames
    double t1 = csvvol->maturities[date1];
    double t2 = csvvol->maturities[date2];
	if(t1 == date.difference) return v1;
	if(t2 == date.difference) return v2;
      double rv = (v1*(date.difference-t1) + v2*(t2-date.difference))/(t2-t1);
    return rv;
  }

double* _get_bucket(volsurface *csvvol, double date)
{


}

double bump_surface(double bumpamount)
{


}

double bump_bucket(double date,double bumpamount)
{


}

//extra stuff - for more exotic timings....

char* check_for_arbitrage()
{

  return NULL;
}


//need interpolation libraries here also for processing sigma(t) term in the PDE
void main()
{
//voltest
   printf("Starting....\n");
   volsurface v;
   initializevol(&v);
   printf("Initialize vol surface....\n");
   v.set_size(&v,13,19);
	printf("Size set\n");
   v.fetch_volatility_surface(&v,"volsurface.csv");
   printf("Loaded & Ready to test...\n");
 //  printf("%f",v.volatility[2][3]);
   qdate q1;
   initqdate(&q1);
   q1.initrefdate(&q1,2012,1,1);
   q1.initdate(&q1,2012,4,1);
   double test=v.get_vol(&v,q1,115);
	printf("interpolated value:%f %f",test,v.strikes[12]);
   
}
