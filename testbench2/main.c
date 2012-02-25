#include <stdio.h>
#include "model.h"

void main()
{

  volsurface v;
  //  interestrates intcurve;
  //load these from some csv file or some shortcut method....
  initialize_volsurface(&v);
 v.set_size(&v,13,19); 
  v.fetch_volatility_surface(&v,"volsurface.csv");
 
  
blackscholes1d autocall;
  initialize_blackscholes1d(&autocall);

  boundaryconditions1D bcs;
  initialize_boundaryconditions1D(&bcs,6);
  int i;
  for(i=0;i<6;i++)
    {
      bcs.time[i] = 3.0-(i) *0.25;
      bcs.value[i] = 30 + (6-i)*5;
      bcs.barrier[i] = 120;
    }

  autocall.apply_boundary_conditions(&autocall,bcs);

  autocall.set_vol_surface(&autocall,v);


  //setintcurves(intcurve);
  results autocallresult = autocall.solve(&autocall);
 // printf("%f",autocallresult.prices[100][1000]);
 // printf("DDDDDAAADDDD");exit(0);
 
  //done and done - that's all!!

}
