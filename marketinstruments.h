
/* 
 Copyright (C) 2012 Animesh Saxena

 This file is part of TradeAnt, a free-software/open-source library
 for traders and practical quantitative analysts

 TradeAnt is free software: you can redistribute it. Use it at your own risk
*/

#ifndef _marketinstruments_
#define _marketinstruments_
#define RELSPOT 100
double _gblackscholes(char dtype,double spot,double strike,double maturity,double sigma,double r);

typedef struct _market_instruments
{
	double strike;
	double absolute_strike;
	double currentspot;

	double marketprice;
	double price;

	char dtype;
	double qty;
	double volatility;
	double maturity;
	double rate;
        void (*add_instrument)(struct _market_instruments*,double,char,double,double,double,double);
	void (*set_spot)(struct _market_instruments*,double);
	
}market_instruments;

void initialize_market_instruments(market_instruments*);
void _add_instrument(market_instruments*,double,char,double,double,double,double);
void _set_spot(market_instruments*,double);

#endif

