#include <stdio.h>
#include <stdlib.h>

#include <tradeant/blackscholes1d.h>
double dt = 0.001;
//sample templates
double max(double a,double b)
{
    if(a>b) return a;
    return b;
}
double callspread_cashflow(double spot,int j)
{
    int p_i = 0;

    int nts = 3.0/dt;
    int dtstep = 0.25*nts/3.0;

                        for(;p_i<=nts;p_i+=dtstep)
                        {
                            if(j==p_i && ((spot)>=97 && (spot)<103))
                            {
                                return 3.0;
                            }

                            else if(j==p_i && ((spot)>=103))
                                return 5.0;
                        }
    return 0;
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

//v.constantvolatility = 0.20;
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


  callspread.set_model_parameters(&callspread,dt,3.0,4,200);
  callspread.set_vol_surface(&callspread,v);
  callspread.set_rates(&callspread,r,divs);

  callspread.apply_cashflow = callspread_cashflow;
  results callspreadout = callspread.solve(&callspread);


int i=0,j=0;

j=callspread.nts-1;
printf("%.4f\n",callspreadout.prices[25][j]);

  for(i=25;i<50;i++)
  {
        printf("%d,%.4f\n",(i*4),1*(callspreadout.prices[i][j]));
  }


}
/*      printf("Delta:%d-->%.5f\n",i,autocallcalloutput.delta[100][j]);
        printf("Gamma:%d-->%.5f\n",i,autocallcalloutput.gamma[100][j]);
        printf("Vega%d-->%.2f\n",i,autocallcalloutput.vega[100][j]);
        printf("Theta%d-->%.2f\n",i,autocallcalloutput.theta[100][j]);
*/



