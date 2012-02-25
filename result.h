/*
 Copyright (C) 2012 Animesh Saxena

 This file is part of TradeAnt, a free-software/open-source library
 for traders and practical quantitative analysts

 TradeAnt is free software: you can redistribute it. Use it at your own risk
*/

typedef struct _results
{
  double **prices;
  double **delta;
  double **gamma;
  double **theta;
  double **vega;
}results;
