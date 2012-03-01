#include <stdio.h>
#include <tradeant/blackscholes1d.h>

double custom_cashflow(double spot,double time)
{
    double a;
    if(spot < 80) a = 80 - spot; else a = 0;
    //return 1;
    if(time==0.0) return -1*a;
}
int main()
{
  //can be today or any other date - format is year,month,day
  setreftime(2012,Jan,1);
  volsurface v;
  initialize_volsurface(&v);
  //v.constantvolatility=0.20;

  v.scale = 0.01;
  v.set_size(&v,21,14);
  v.fetch_volatility_surface(&v,"volsurface.csv");
  int i=0;

  rates r;
  initialize_rates(&r);
  r.constantrate = 0.05;

  rates divs;
  initialize_rates(&divs);
  divs.constantrate = 0.001;

  blackscholes1d autocall;
  initialize_blackscholes1d(&autocall);
  autocall.stepsize = 1;
  autocall.numberofsteps = 200;
  autocall.expiry = 3.0;
  autocall.dt = 0.03;
  //v.constantvolatility = 0.20;
  autocall.set_vol_surface(&autocall,v);

  autocall.set_rates(&autocall,r,divs);

 cashflows1d bcs;
 initialize_cashflows1d(&bcs,6);

  for(i=0;i<6;i++)

    {

      bcs.time[i] = 3.0-(i) *0.25;

      bcs.value[i] = 30 + (6-i)*5;

      bcs.barrier[i] = 120;

    }

autocall.add_cash_flows(&autocall,bcs);
autocall.apply_cashflow = custom_cashflow;

results autocallresult = autocall.solve(&autocall);
printf("\n%.5f\n",autocallresult.prices[100][autocall.nts-10]);
//printf("DDD");

return 0;
}
