/*
 Copyright (C) 2012 Animesh Saxena

 This file is part of TradeAnt, a free-software/open-source library
 for traders and practical quantitative analysts

 TradeAnt is free software: you can redistribute it. Use it at your own risk
*/
#ifndef _results_
#define _results_

typedef struct _results
{
  double **prices;
  double **delta;
  double **gamma;
  double **theta;
  double **vega;

}results;

typedef struct _results2d
{
  double *prices;
  double *delta;
  double *gamma;
  double *theta;
  double *vega;

  int i,j,k;

  double (*get_prices)(struct _results2d*,int,int,int);
  double (*get_delta)(struct _results2d*,int,int,int);
  double (*get_theta)(struct _results2d*,int,int,int);
  double (*get_vega)(struct _results2d*,int,int,int);
  double (*get_gamma)(struct _results2d*,int,int,int);

}results2d;


void _init_results(results *_output,int numberofsteps,int nts);
void _init_results2d(results2d* _output,int numberofsteps,int nts);

  double _get_prices(struct _results2d*,int,int,int);
  double _get_delta(struct _results2d*,int,int,int);
  double _get_theta(struct _results2d*,int,int,int);
  double _get_vega(struct _results2d*,int,int,int);
  double _get_gamma(struct _results2d*,int,int,int);

double _get_array(int con,results2d*,int i,int j,int k);
#endif
