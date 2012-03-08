
/*
 Copyright (C) 2012 Animesh Saxena

 This file is part of TradeAnt, a free-software/open-source library
 for traders and practical quantitative analysts

 TradeAnt is free software: you can redistribute it. Use it at your own risk
*/


#include <stdio.h>
#include <tradeant/blackscholes2d.h>


double bestofcall_cashflow(double spot[],double time)
{
    double a;
    if(spot[0] > spot[1] && time == 0.0)
        if(spot[0] > 100) return (spot[0]-100.0);

    if(spot[1] > spot[0] && time == 0.0)
        if(spot[1] > 100) return (spot[1]-100.0);

    return 0.0;
}

int main()
{
  //can be today or any other date - format is year,month,day
  setreftime(2012,Jan,1);

  //Volatility Surface
  volsurface v[2];
  rates r;
  initialize_rates(&r);
  r.set_constant_rate(&r,0.05);

 //Dividend Curve
  rates divs[2];


  int i=0;
  for(;i<2;i++)
  {
        initialize_volsurface(&v[i]);
        v[i].scale = 0.01;
        v[i].set_size(&v[i],21,14);
        v[i].constantvolatility = 0.20;
        //v[i].fetch_volatility_surface(&v[i],"volsurface.csv");

        initialize_rates(&divs[i]);
        divs[i].set_constant_rate(&divs[i],0.001);
  }

  blackscholes2d bestofcall;
  initialize_blackscholes2d(&bestofcall);

  bestofcall.set_model_parameters(&bestofcall,0.01,1.0,1.0,200,0.0);
  bestofcall.set_vol_surface(&bestofcall,v);
  bestofcall.set_rates(&bestofcall,r,divs);

  bestofcall.apply_cashflow = bestofcall_cashflow;
  results2d bestofcalloutput = bestofcall.solve(&bestofcall);
  for(i=1;i<bestofcall.numberofsteps;i++)
        printf("Price:%.3f\t",bestofcalloutput.get_prices(&bestofcalloutput,10,i,bestofcall.nts-1));
return 0;
}
