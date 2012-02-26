#include <stdio.h>
#include <tradeant/blackscholes1d.h>


int main()
{
//can be today or any other date - format is year,month,day
  setreftime(2012,Jan,1);
  volsurface v;
  initialize_volsurface(&v);
  v.constantvolatility = 0.20;
  rates r;
  initialize_rates(&r);
  r.constantrate = 0.05;
  blackscholes1d autocall;
  autocall.expiry = 3.0;
  autocall.dt = 0.001;
  autocall.stepsize = 1;
  autocall.numberofsteps = 200;


  initialize_blackscholes1d(&autocall);

  cashflows1d bcs;
  initialize_cashflows1d(&bcs,6);
  int i;
  for(i=0;i<6;i++)
    {
      bcs.time[i] = 3.0-(i) *0.25;
      bcs.value[i] = 30 + (6-i)*5;
      bcs.barrier[i] = 120;
    }

  autocall.add_cash_flows(&autocall,bcs);
  autocall.set_vol_surface(&autocall,v);
  autocall.set_rates(&autocall,r,r);

  results autocallresult = autocall.solve(&autocall);
int j;
for(j=1;j<autocall.nts;j++)
{
	for(i=1;i<200;i++)
	{
	    printf("%.4f\t",autocallresult.delta[i][j]);
	}
	printf("\n");
}


  //done and done - that's all!!
/*market_instruments m;
initialize_market_instruments(&m);

m.add_instrument(&m,5000,'P',300,1,1,0.05);
m.set_spot(&m,5000);

//double aa1 = _gblackscholes('P',100,100,1,0.20,0);
//printf("%.3f",aa1);
printf("%.3f",m.volatility);
*/
return 0;
}
