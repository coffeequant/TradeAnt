#include <stdio.h>
#include <tradeant/blackscholes1d.h>


//sample templates
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
  volsurface v;
  initialize_volsurface(&v);

  v.scale = 0.01;
  v.set_size(&v,21,14);
  v.fetch_volatility_surface(&v,"volsurface.csv");
  int i=0;

  rates r;
  initialize_rates(&r);
  r.constantrate = 0.01;

  rates divs;
  initialize_rates(&divs);
  divs.constantrate = 0.01;


  blackscholes1d digital;
  initialize_blackscholes1d(&digital);
  digital.stepsize = 1;
  digital.numberofsteps = 200;
  digital.expiry = 1.25;
  digital.dt = 0.005;
  digital.set_vol_surface(&digital,v);
  digital.set_rates(&digital,r,divs);


digital.apply_cashflow = digital_cashflow;

results digitalcalloutput = digital.solve(&digital);

digital.apply_cashflow = digital2_cashflow;

results digitalputoutput = digital.solve(&digital);

for(i=0;i<digital.nts-1;i++)
    printf("%.2f,",digitalcalloutput.delta[100][i]);


return 0;
}
