#include <stdio.h>
#include <stdlib.h>
#include <tradeant/blackscholes1d.h>

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

//sample templates

double downinput(double spot,double timeremaining)

{

    double payoff = 0.0;

    if(spot <= 60 && timeremaining == 0.0) payoff = -1*(100 - spot);

    return payoff;

}

double digital_cashflow(double spot,double timeremaining)

{

    double payoff = 0.0;
    if(spot >=105 && timeremaining == 0.0) payoff = MAX(0.0,spot - 105.0);
    return payoff;
}


double digital2_cashflow(double spot,double timeremaining)

{

    //return 1;

    double a;

    if(spot >= 100.0 && timeremaining == 0.0)

        return 13;

    else return 0.0;

}


int main(int argc,char *argv[])
{

//First of all set a reference time from which all times will be calculated. Generally it is today but here you can set it to anything...
  //can be today or any other date - format is year,month,day
  setreftime(2022,2,10);
  //Volatility Surface
  volsurface v;
  initialize_volsurface(&v);

//  v.scale = 1;
//  v.constantvolatility = 0.20;

  v.set_size(&v,55,9);
  v.fetch_volatility_surface(&v,"volsurface.csv");


  //Interest Rate Curve
  rates r;
  initialize_rates(&r);
  r.set_constant_rate(&r,0.0);
	
  //Dividend Curve
  rates divs;
  initialize_rates(&divs);
  divs.set_constant_rate(&divs,0.0);

  blackscholes1d digital;
  initialize_blackscholes1d(&digital);

  //timeslice, expiry,stepsize,numberofspots

  digital.set_model_parameters(&digital,0.005,1.00,1,400);
  digital.set_vol_surface(&digital,v);
  digital.set_rates(&digital,r,divs);


  digital.apply_cashflow = digital_cashflow;
  results digitalcalloutput = digital.solve(&digital);



int i=0,j=0;

//printf("\n*************Digital Call Prices***********\n");

j=digital.nts-1;

//printf("%.2f\n\n",digitalcalloutput.prices[100][j]);

for(i=0;i<digital.numberofsteps;i++)
{
	printf("%.2f,",digitalcalloutput.vega[i][j]);
}
//printf("\n\n\n");
return 0;
}


