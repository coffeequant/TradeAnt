#include <stdio.h>
#include "model.h"


int main()
{
//can be today or any other date - format is year,month,day
  setreftime(2012,Jan,1);

  volsurface v;
  initialize_volsurface(&v);
  v.set_size(&v,13,19); 
  v.fetch_volatility_surface(&v,"volsurface.csv");
 
  rates r;
  initialize_rates(&r);
  r.set_size(&r,19); 
  r.fetch_rates(&r,"intrates.csv");
  blackscholes1d autocall;
  initialize_blackscholes1d(&autocall);
  autocall.expiry = 3.0;

  cashflows1d bcs;
  initialize_cashflows1d(&bcs,6);
  int i;
  for(i=0;i<6;i++)
    {
      bcs.time[i] = 3.0-(i) *0.25;
      bcs.value[i] = 30 + (6-i)*5;
      bcs.barrier[i] = 110;
    }

  autocall.add_cash_flows(&autocall,bcs);
  autocall.set_vol_surface(&autocall,v);
  autocall.set_rates(&autocall,r,r);

  //setintcurves(intcurve);

  results autocallresult = autocall.solve(&autocall);

  printf("%f",autocallresult.prices[100][autocall.nts-1]);
 // printf("DDDDDAAADDDD");exit(0);


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
