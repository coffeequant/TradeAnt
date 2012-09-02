#include <stdio.h>
#include <stdlib.h>

#include <tradeant/blackscholes1d.h>

//sample templates
double max(double a,double b)
{
    if(a>b) return a;
    return b;
}
double callspread_cashflow(double spot,double timeremaining)
{
    if(timeremaining == 0.0)
    {
        double buycallprice = max(spot-99,0);
        double sellcallprice = max(spot-100,0);
        return 15*(buycallprice-sellcallprice);
    }
    return 0;
}

double digital_cashflow(double spot,double timeremaining)
{

    double payoff = -1;
    double barrier = 100;

    if(spot > barrier && timeremaining == 0.0) return 15;

    return payoff;

}

int main()
{


//First of all set a reference time from which all times will be calculated. Generally it is today but here you can set it to anything...
  //can be today or any other date - format is year,month,day
  setreftime(2012,Jun,10);

  //Volatility Surface
  volsurface v;
  initialize_volsurface(&v);

  v.scale = 1;
  v.set_size(&v,55,10);
  //v.constantvolatility = 0.20;
  v.fetch_volatility_surface(&v,"volsurface.csv");

//v.constantvolatility = 0.21;
//Interest Rate Curve

  rates r;
  initialize_rates(&r);
  r.set_constant_rate(&r,0.0);

  //Dividend Curve
  rates divs;
  initialize_rates(&divs);
  divs.set_constant_rate(&divs,0);

  blackscholes1d digital;
  initialize_blackscholes1d(&digital);

  blackscholes1d callspread;
  initialize_blackscholes1d(&callspread);



  digital.set_model_parameters(&digital,0.005,1.0,1,200);
  digital.set_vol_surface(&digital,v);
  digital.set_rates(&digital,r,divs);

 callspread.set_model_parameters(&callspread,0.005,1.0,1,200);
  callspread.set_vol_surface(&callspread,v);
  callspread.set_rates(&callspread,r,divs);


  digital.apply_coupon = digital_cashflow;
  results digitalout = digital.solve(&digital);

  callspread.apply_cashflow = callspread_cashflow;
  results callspreadout = callspread.solve(&callspread);


int i=0,j=0;

j=100;
i=100;

//printf("%d,%.4f\n",i,1*(callspreadout.prices[199][10]));
//exit(0);

  for(i=0;i<digital.numberofsteps-1;i++)
  {
        printf("%d,%.4f\n",i,1*(digitalout.prices[i+1][j]));
  }
        printf("%d,%.4f\n",i,100*0);
        printf("\n");

}
/*      printf("Delta:%d-->%.5f\n",i,autocallcalloutput.delta[100][j]);
        printf("Gamma:%d-->%.5f\n",i,autocallcalloutput.gamma[100][j]);
        printf("Vega%d-->%.2f\n",i,autocallcalloutput.vega[100][j]);
        printf("Theta%d-->%.2f\n",i,autocallcalloutput.theta[100][j]);
*/



