#include <stdio.h>

#include <tradeant/blackscholes1d.h>

//sample templates


double autocall_cashflow(double spot,double timeremaining)

{

    double payoff = -1;

    if(spot > 105 && timeremaining == 0.0) return 51;
    if(spot > 105 && timeremaining == 0.25) return 46.75;
    if(spot > 105 && timeremaining == 0.5) return 42.5;
    if(spot > 105 && timeremaining == 0.75) return 38.25;
    if(spot > 105 && timeremaining == 1.0) return 34;
    if(spot > 105 && timeremaining == 1.25) return 29.75;
    if(spot > 105 && timeremaining == 1.5) return 25.5;
    if(spot > 105 && timeremaining == 1.75) return 21.25;
    if(spot > 105 && timeremaining == 2.0) return 17;

    if(spot <= 80) payoff = 6.5*(3-timeremaining);

    return payoff;

}


int main()
{


//First of all set a reference time from which all times will be calculated. Generally it is today but here you can set it to anything...
  //can be today or any other date - format is year,month,day
  setreftime(2012,May,10);

  //Volatility Surface
  volsurface v;
  initialize_volsurface(&v);

  v.scale = 1;
  v.set_size(&v,70,9);
//  v.fetch_volatility_surface(&v,"volsurface.csv");
  v.constantvolatility = 0.21;

  //Interest Rate Curve
  rates r;
  initialize_rates(&r);
  r.set_constant_rate(&r,0.0);

  //Dividend Curve
  rates divs;
  initialize_rates(&divs);
  divs.set_constant_rate(&divs,0.0);

  blackscholes1d autocall;
  initialize_blackscholes1d(&autocall);

  autocall.set_model_parameters(&autocall,0.005,3.0,1,200);
  autocall.set_vol_surface(&autocall,v);
  autocall.set_rates(&autocall,r,divs);

  autocall.apply_coupon = autocall_cashflow;
  results autocallcalloutput = autocall.solve(&autocall);


int i=0,j=0;

printf("\n*************autocall Call Prices***********\n");
j=autocall.nts-1;
{
  for(i=0;i<autocall.numberofsteps;i++)
  {
        printf("%d-->%.2f %.2f\t",i,autocallcalloutput.prices[i][j],autocallcalloutput.vega[i][j]);
  }
        printf("\n");
}

}


