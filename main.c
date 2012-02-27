#include <stdio.h>
#include <tradeant/blackscholes2d.h>

double apply_custom_cashflow(double spot[],double time)
{
    double m = (spot[0]>spot[1])?spot[0]:spot[1];
    //printf("%.2f\n",((m>100)?(m-100):0.0));
    if(time == 0.0)
    {
        //exit(0);
        return ((m>100)?(m-100):0.0);
    }


    return 0.0;
}

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
  blackscholes2d bestofcall;
  bestofcall.expiry = 1.0;
  bestofcall.dt = 0.01;
  bestofcall.stepsize = 1;
  bestofcall.numberofsteps = 200;


  initialize_blackscholes2d(&bestofcall);
  bestofcall.apply_cashflow = apply_custom_cashflow;
  results2d bestofcallresult = bestofcall.solve(&bestofcall);
int j;
for(j=0;j<100;j++)
  //printf("%.5f\n",bestofcallresult.prices[100*100*99+j]);
	printf("%.5f\n",bestofcallresult.prices[bestofcall.nts*200*199+100]);
return 0;
}
