
/*
 Copyright (C) 2012 Animesh Saxena

 This file is part of TradeAnt, a free-software/open-source library
 for traders and practical quantitative analysts

 TradeAnt is free software: you can redistribute it. Use it at your own risk
*/


#include <stdio.h>
#include <tradeant/blackscholes1d.h>


//sample templates
double downinput(double spot,double timeremaining)
{
    double payoff = 0.0;
    if(spot <= 60 && timeremaining == 0.0) payoff = -1*(100 - spot);
    return payoff;
}
double digital_cashflow(double spot,double timeremaining)
{
    double a;
    if(spot < 90 && timeremaining == 0.0) return 13;
    else return 0.0;
}

double digital2_cashflow(double spot,double timeremaining)
{
    //return 1;
    double a;
    if(spot >= 100.0 && timeremaining == 0.0)
        return 13;
    else return 0.0;
}


int main()
{
  //can be today or any other date - format is year,month,day
  setreftime(2012,Jan,1);

  //Volatility Surface
  volsurface v;
  initialize_volsurface(&v);

  v.scale = 0.01;
  v.set_size(&v,21,14);
  v.constantvolatility = 0.21;
  v.fetch_volatility_surface(&v,"volsurface.csv");


  //Interest Rate Curve
  rates r;
  initialize_rates(&r);
  r.set_constant_rate(&r,0.0);

  //Dividend Curve
  rates divs;
  initialize_rates(&divs);
  divs.set_constant_rate(&divs,0.0);

  blackscholes1d digital;
  initialize_blackscholes1d(&digital);

  digital.set_model_parameters(&digital,0.005,1.25,1,200);
  digital.set_vol_surface(&digital,v);
  digital.set_rates(&digital,r,divs);


  digital.apply_cashflow = digital_cashflow;
  results digitalcalloutput = digital.solve(&digital);

  digital.apply_cashflow = digital2_cashflow;
  results digitalputoutput = digital.solve(&digital);


int i=0,j=0;

printf("\n*************Digital Call Prices***********\n");
j=digital.nts-1;
{
  for(i=0;i<digital.numberofsteps;i++)
  {
        printf("%.2f\t",digitalcalloutput.prices[i][j]);
  }
        printf("\n");
}
j=0;
printf("\n*************Digital Call Deltas***********\n");
j=digital.nts-1;
{
  for(i=0;i<digital.numberofsteps;i++)
  {
        printf("%.2f\t",digitalcalloutput.delta[i][j]);
  }
        printf("\n");
}
j=0;
printf("\n*************Digital Call Gammas***********\n");
j=digital.nts-1;
{
  for(i=0;i<digital.numberofsteps;i++)
  {
        printf("%.2f\t",digitalcalloutput.gamma[i][j]);
  }
        printf("\n");
}

//let's move on to something more exotic
//an autocall which pays 20% annualized coupon
//if a barrier is hit on specific times
//if none of the barriers are hit but at expiry index is below 60% the buyer loses 100 - x% (where x is below 60)

blackscholes1d autocall;
initialize_blackscholes1d(&autocall);
autocall.set_model_parameters(&autocall,0.01,3.0,1,200);
autocall.set_vol_surface(&autocall,v);
autocall.set_rates(&autocall,r,divs);

cashflows1d coupons;
initialize_cashflows1d(&coupons,6);
 for(i=0;i<6;i++)
    {
      coupons.time[i] = 3.0-(i) *0.25;
      coupons.value[i] = 30 + (6-i)*5;
      coupons.barrier[i] = 120;
    }

autocall.add_cash_flows(&autocall,coupons);

autocall.apply_cashflow = downinput;
results autocalloutput = autocall.solve(&autocall);

printf("\n*************Autocallable Prices***********\n");
j=autocall.nts-1;
{
  for(i=100;i<130;i++)
  {
        printf("Spot %d Price -- %.2f \n",i,autocalloutput.prices[i][j]);
  }
        printf("\n");
}

printf("\n*************Autocallable Deltas***********\n");
j=autocall.nts-1;
{
  for(i=100;i<130;i++)
  {
        printf("Spot %d Delta -- %.2f \n",i,autocalloutput.delta[i][j]);
  }
        printf("\n");
}

printf("\n*************Autocallable Gammas***********\n");
j=autocall.nts-1;
{
  for(i=100;i<130;i++)
  {
        printf("Spot %d Gamma -- %.2f \n",i,autocalloutput.gamma[i][j]);
  }
        printf("\n");
}

return 0;
}
