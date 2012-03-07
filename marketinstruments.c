/*
 Copyright (C) 2012 Animesh Saxena

 This file is part of TradeAnt, a free-software/open-source library
 for traders and practical quantitative analysts

 TradeAnt is free software: you can redistribute it. Use it at your own risk
*/

#include <stdlib.h>
#include <tradeant/marketinstruments.h>
#include <gsl/gsl_cdf.h>
#include <math.h>

/*
Each structure has an initialization function which sets up the function pointers. This is similar to what C++ does in the background but virtual table is too messy and memory footprint is too large
*/
void initialize_market_instruments(market_instruments* m)
{
	m->add_instrument = _add_instrument;
	m->set_spot = _set_spot;
}
double _gblackscholes(char dtype,double spot,double strike,double maturity,double sigma,double r)
{
	double d1 = (log(spot/strike) + (r +0.5 * sigma * sigma)*maturity)/(sigma*pow(maturity,0.5));
	double d2 = d1 - sigma * pow(maturity,0.5);
	double price = 0.0;
	if(dtype == 'C')
		price = spot * gsl_cdf_ugaussian_P(d1) - strike * exp(-r*maturity)*gsl_cdf_ugaussian_P(d2);
	else
		price = -1*spot * gsl_cdf_ugaussian_P(-1*d1) + strike * exp(-r*maturity)*gsl_cdf_ugaussian_P(-1*d2);

	return price;
}

void _add_instrument(market_instruments* m,double strike, char dtype, double marketprice,double expiry,double qty,double r)
{
	m->strike = strike;
	m->dtype = dtype;
	m->marketprice=marketprice;
	m->maturity = expiry;
	m->qty = qty;
	m->rate = r;
}

void _set_spot(market_instruments* m,double spot)
{
	//calculate implied vol - always keep relative prices coz it makes the hedging equations easier.
	m->price = (m->marketprice)/(spot) * 100.0;
	m->absolute_strike = m->strike;
	m->strike = (m->strike/spot)*100.0;
	double sigmalow = 0.005;
	double sigmahigh = 2;
        m->currentspot = spot;


	double error = 0.0005;
	double pricelow = _gblackscholes(m->dtype,RELSPOT,m->strike,m->maturity,sigmalow,m->rate);
	double pricehigh = _gblackscholes(m->dtype,RELSPOT,m->strike,m->maturity,sigmahigh,m->rate);

	double sigma = sigmalow + (m->price - pricelow) * (sigmahigh - sigmalow) / (pricehigh - pricelow);
	while(abs(m->price - _gblackscholes(m->dtype,RELSPOT,m->strike,m->maturity,sigma,m->rate)) > error)
	{
		if( _gblackscholes(m->dtype,RELSPOT,m->strike,m->maturity,sigma,m->rate) < m->price)
			sigmalow = sigma;
		else
			sigmahigh = sigma;
		pricelow =  _gblackscholes(m->dtype,RELSPOT,m->strike,m->maturity,sigmalow,m->rate);
		pricehigh =  _gblackscholes(m->dtype,RELSPOT,m->strike,m->maturity,sigmahigh,m->rate);
		sigma = sigmalow + (m->price - pricelow) * (sigmahigh - sigmalow) / (pricehigh - pricelow);
	}

	m->volatility = sigma;
}


