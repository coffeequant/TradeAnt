
/*
 Copyright (C) 2012 Animesh Saxena

 This file is part of TradeAnt, a free-software/open-source library
 for traders and practical quantitative analysts

 TradeAnt is free software: you can redistribute it. Use it at your own risk
*/


#include <stdio.h>
#include <tradeant/heston1d.h>


double vanillacall_cashflow(double spot,double time)
{

    if(spot > 100 && time == 0.0)
    {
        return spot-100.0;
    }

    if(time == 0.0)
        return 0.0;

}

double vanillacall_coupon(double spot,double time)
{

double t = 3.0;
double a = 0.0;
    if(spot > 120 && time == (t-3.0))
        return 60.0;
    else if(spot > 120 && time == (t-2.75))
        return 55.0;
    else if(spot > 120 && time == (t-2.5))
        return 50.0;
    else if(spot > 120 && time == (t-2.25))
        return 45.0;
    if(spot > 120 && time == (t-2.0))
        return 40.0;
    if(spot > 120 && time == (t-1.75))
        return 35.0;

    if(time == 0.0 && spot <= 120)
        a=0.0;
        return a;

}


int main()
{
  //can be today or any other date - format is year,month,day
  setreftime(2012,Jan,1);

  rates r;
  initialize_rates(&r);
  r.set_constant_rate(&r,0.07);



 //Dividend Curve
  rates divs;

  initialize_rates(&divs);
  divs.set_constant_rate(&divs,0.0);

  heston1d vanillacall;
  initialize_heston1d(&vanillacall);

  vanillacall.set_model_parameters(&vanillacall,0.05,3,1.0,200,0.005,200);

/*hs->correlation = correlation;
    hs->meanreversion = meanreversion;
    hs->longtermvariance = longtermvariance;
    hs->volofvol = volofvol;*/

  vanillacall.set_calibration_parameters(&vanillacall,-0.5,0.3,0.2,0.9);
  vanillacall.set_rates(&vanillacall,r,divs);

  //vanillacall.apply_cashflow = vanillacall_cashflow;
  vanillacall.apply_coupon = vanillacall_coupon;
  results2d vanillacalloutput = vanillacall.solve(&vanillacall);

  int nas1 = 200;
  int nas2 = 200;
int i;
int j = 40;
  for(i=100;i<150;i++)
        printf("Prie:%d %.5f\t",i,vanillacalloutput.prices[(vanillacall.nts-1)*nas1*nas2+nas2*j+i]);
return 0;
}
