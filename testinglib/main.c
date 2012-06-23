#include <stdio.h>

#include <tradeant/blackscholes1d.h>

//sample templates


double autocall_cashflow(double spot,double timeremaining)

{

    double payoff = -1;
    double barrier = 110;


    if(spot > barrier && timeremaining == 0.0) return 48;
    if(spot > barrier && timeremaining == 0.25) return 44;
    if(spot > barrier && timeremaining == 0.5) return 40;
    if(spot > barrier && timeremaining == 0.75) return 36;
    if(spot > barrier && timeremaining == 1.0) return 32;
    if(spot > barrier && timeremaining == 1.25) return 28;
    if(spot > barrier && timeremaining == 1.5) return 24;
    if(spot > barrier && timeremaining == 1.75) return 20;

    return payoff;

}


int main()
{


//First of all set a reference time from which all times will be calculated. Generally it is today but here you can set it to anything...
  //can be today or any other date - format is year,month,day
  setreftime(2012,Jun,22);

  //Volatility Surface
  volsurface v;
  initialize_volsurface(&v);

  v.scale = 1;
  v.set_size(&v,55,10);
  v.constantvolatility = 0.20;
  //v.fetch_volatility_surface(&v,"volsurface.csv");

//v.constantvolatility = 0.21;
//Interest Rate Curve

  rates r;
  initialize_rates(&r);
  r.set_constant_rate(&r,0.0);

  //Dividend Curve
  rates divs;
  initialize_rates(&divs);
  divs.set_constant_rate(&divs,-0.03);

  blackscholes1d autocall;
  initialize_blackscholes1d(&autocall);

  autocall.set_model_parameters(&autocall,0.001,3.0,1,200);
  autocall.set_vol_surface(&autocall,v);
  autocall.set_rates(&autocall,r,divs);

  autocall.apply_coupon = autocall_cashflow;
  results autocallcalloutput = autocall.solve(&autocall);

int i=0,j=0;

printf("\n*************autocall Call Prices***********\n");
j=autocall.nts-1;
i=100;
{
  //for(i=0;i<autocall.numberofsteps;i++)
  //{
        printf("Price:%d-->%.2f\n",i,autocallcalloutput.prices[100][j]);
        printf("Delta:%d-->%.5f\n",i,autocallcalloutput.delta[100][j]);
        printf("Gamma:%d-->%.5f\n",i,autocallcalloutput.gamma[100][j]);
        printf("Vega%d-->%.2f\n",i,autocallcalloutput.vega[100][j]);
        printf("Theta%d-->%.2f\n",i,autocallcalloutput.theta[100][j]);

  //}
        printf("\n");
}

}


