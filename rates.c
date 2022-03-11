/*
 Copyright (C) 2012 Animesh Saxena

 This file is part of TradeAnt, a free-software/open-source library
 for traders and practical quantitative analysts

 TradeAnt is free software: you can redistribute it. Use it at your own risk
*/

#include <stdio.h>
#include <stdlib.h>
#include <tradeant/dates.h>
#include <tradeant/rates.h>
#include <string.h>

#define LINELENGTH 1000
#define SEPARATOR ","
#define RATECOOKIE "ANIRATE"

struct tm _globalref;
//need interest rate and dividend curves both
//taken from volsurface file
double interpolate(double*,double*,double,int);

void initialize_rates(rates* r)
{
	r->fetch_rates = _fetch_rates;
	r->get_rate_with_date = _get_rate_with_date;
	r->get_rate_with_reftime = _get_rate_with_reftime;
	r->bump_rates_with_date = _bump_rates_with_date;
	r->bump_rates_with_reftime = _bump_rates_with_reftime;
	r->bump_rates = _bump_rates;
	r->set_rates_with_date = _set_rates_with_date;
	r->set_rates_with_reftime = _set_rates_with_reftime;
	r->set_size = _set_rates_size;
    r->set_constant_rate = _set_constant_rate;
	r->isconst = 'n';
}

void _set_constant_rate(rates *r,double rate)
{
    r->isconst='y';
    r->constantrate = rate;
}

void _set_rates_size(rates* r,int c)
{
  extern struct tm _globalref;
	int i;
	r->matcount = c;
	r->rate = (double*)malloc(sizeof(double)*c);
	r->maturities = (double*)malloc(sizeof(double)*c);
        r->absolute_maturities = (qdate*)malloc(sizeof(qdate)*c);
  	for(i=0;i<c;i++)
	{
		initqdate(&(r->absolute_maturities[i]));
		r->absolute_maturities[i].initrefdate(&r->absolute_maturities[i],_globalref.tm_year+1900,_globalref.tm_mon,_globalref.tm_mday);
  	}


}

void _fetch_rates(rates* r,char* filename)
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
       if(strstr(newline,RATECOOKIE)==NULL)
       {
        printf("Error in Rates\n");
        exit(1);
      	}
	 int i=0;

     while(i<r->matcount-1)
	{
		fgets(newline,LINELENGTH,fp);
	      	char *date;
		date=strtok(newline,SEPARATOR);
		r->rate[i] = atof(strtok(NULL,SEPARATOR));
	      	r->absolute_maturities[i].initstringdate(&r->absolute_maturities[i],date);
		r->maturities[i] = r->absolute_maturities[i].difference;
		i++;
	}
     }

}
double _get_rate_with_date(rates* r,qdate date)
{
    if(r->isconst == 'y')
        return r->constantrate;

	    int i=0;
	    double *v1array = (double*)malloc(sizeof(double)*(r->matcount));
            double *v2array = (double*)malloc(sizeof(double)*(r->matcount));
	for(i=0;i<r->matcount;i++)
	{
		v1array[i] = r->maturities[i];
		v2array[i] = r->rate[i];
	}
	double r1 = interpolate(v1array,v2array,date.difference,r->matcount);
	return r1;
     //simple interpolation
}
double _get_rate_with_reftime(rates* r,double difference)
{
    if(r->isconst == 'y')
        return r->constantrate;

    int i=0;
	double *v1array = (double*)malloc(sizeof(double)*(r->matcount));
        double *v2array = (double*)malloc(sizeof(double)*(r->matcount));
	for(i=0;i<r->matcount;i++)
	{
		v1array[i] = r->maturities[i];
		v2array[i] = r->rate[i];
	}
	double r1 = interpolate(v1array,v2array,difference,r->matcount);
	return r1;
}

void _set_rates_with_reftime(rates *r,double *difference, double* rate)
{
	int i;
	for(i=0;i<r->matcount;i++)
	{
		r->maturities[i] = difference[i];
		r->rate[i] = rate[i];
	}
	//need to set qdates or absolute_maturities
}
void _set_rates_with_date(rates* r,qdate* a,double* rate)
{
	int i;
	for(i=0;i<r->matcount;i++)
	{
		r->maturities[i] = a[i].difference;
		r->rate[i] = rate[i];
	}
}

void _bump_rates_with_reftime(rates* r,double difference,double bumpamount)
{
    int i=0;
    for(;i<r->matcount;i++)
      if(r->absolute_maturities[i].difference >= difference) goto exitLoop;

  exitLoop:
    i = i - 1;i = i +1;

    int date1 = i;
      r->rate[date1] += bumpamount;

}
void _bump_rates_with_date(rates* r,qdate date,double bumpamount)
{
	    int i=0;
    for(;i<r->matcount;i++)
      if(r->absolute_maturities[i].difference >= date.difference) goto exitLoop;

exitLoop:
        i = i - 1;i = i +1;

    int date1 = i;
      r->rate[date1] += bumpamount;
}
void _bump_rates(rates* r,double bumpamount)
{
 int i=0;
    for(;i<r->matcount;i++)
          r->rate[i] += bumpamount;
}


//how to incorporat discounting...the r(t) term in the equation....
//need interpolation libraries...
//flat or cubic or quadratic??

