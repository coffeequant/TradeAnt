/*
 Copyright (C) 2012 Animesh Saxena

 This file is part of TradeAnt, a free-software/open-source library
 for traders and practical quantitative analysts

 TradeAnt is free software: you can redistribute it. Use it at your own risk
*/

#include <stdio.h>
#include <tradeant/blackscholes1d.h>
#include <math.h>

#include <gsl/gsl_errno.h>
#include <gsl/gsl_linalg.h>

//this file combines all the data and is the main pricing engine
//the non standard PDE would allow other than standard variables to have two extra terms....
//problem is how to decide the order of loops....
//first is always time...but depending on boundary conditions out of 3 which one's come first....


results solvebs_implicit(blackscholes1d*,double);

void initialize_blackscholes1d(blackscholes1d *bs)
{
//	  bs->solve = solvebs_explicit;
	  bs->solve = solvebs;
	  bs->set_vol_surface = _set_vol_surface;
	  bs->set_rates = _set_rates;
	  bs->add_cash_flows = _add_cash_flows;
	  bs->boundarycount = 0;
	  bs->hedge_instruments = _hedge_instruments;
	  bs->apply_cashflow = NULL;
	  bs->apply_coupon = NULL;
	  bs->nts = (int)(bs->expiry / bs->dt);
	  bs->set_model_parameters = _set_model_parameters;
}

void _add_cash_flows(blackscholes1d *bs,cashflows1d modelbc)
{
  	bs->bc[bs->boundarycount++]= modelbc;
}
void _set_vol_surface(blackscholes1d *bs,volsurface v)
{

	bs->_blackscholesvol = v;
}

void _set_rates(blackscholes1d* bs,rates interestrates, rates dividends)
{
	bs->_interestrates = interestrates;
	bs->_dividendrates = dividends;
}

void _set_model_parameters(struct _blackscholes1d* bs,double timeslice,double expiry,double stepsize,int numberofspotsteps)
{
    bs->dt = timeslice;
    bs->expiry = expiry;
    bs->stepsize = stepsize;
    bs->numberofsteps = numberofspotsteps;
}

void _apply_cash_flows(blackscholes1d* bs,int j,results *output)
{
     int g,b,i;
     for(g=0;g<bs->boundarycount;g++)
     {

    	for(b=0;b<bs->bc[g].bccount;b++)
    	{
    	    if((bs->nts-j)*(bs->dt) >= bs->bc[g].time[b])
    		{
            for(i=0;i<bs->numberofsteps;i++)

    			if(i*bs->stepsize >= bs->bc[g].barrier[b])
				{
						output->prices[i][j] = bs->bc[g].value[b];
				}
    		}
    	}
      }

}


void _hedge_instruments(blackscholes1d *bs,double s,double t,results *_output)
{
	//loop thru all market_instruments and
	int i;
	int s_index = s / bs->stepsize;
	int t_index = t / bs->dt;
	market_instruments *m = bs->_m;
	for(i=0;i<bs->_mi_count;i++)
	{
		if(m[i].maturity == t)
		{
			if(m[i].dtype == 'C')
                           _output->prices[s_index][t_index] += m[i].qty*fmax(s-m[i].strike,0);
			if(m[i].dtype == 'P')
				_output->prices[s_index][t_index] += m[i].qty*fmax(m[i].strike - s,0);
		}
	}
}


//activate the hedge_instruments function call which applies the call and put cash flows when the PDE is solved.
results hedge_exotic(blackscholes1d* bs,market_instruments *m,int count)
{
	bs->hedge_instruments = _hedge_instruments;
	bs->_m = m;
	bs->_mi_count = count;
	results hedges = solvebs(bs);
	bs->hedge_instruments = NULL;
	return hedges;
}

results solve_experimental(blackscholes1d* bs,double increment)
{

    double dt = bs->dt;
    int i,j;

    double ds = bs->stepsize;
    int nts = bs->expiry/dt;
    int nas = bs->numberofsteps;
    bs->nts = nts;
    results _output;

    _init_results(&_output,bs->numberofsteps,bs->nts);
    double *Au = (double*)malloc(sizeof(double)*nas);
    double *Bu = (double*)malloc(sizeof(double)*nas);
    double *Du = (double*)malloc(sizeof(double)*nas);
    double *Eu = (double*)malloc(sizeof(double)*nas);
    double *Fu = (double*)malloc(sizeof(double)*nas);
    double volatility,intrate,divrate;


    for(j=0;j<nts;j++)
    {

        if(j%2==0)
        {
            //upstream
            for(i=0;i<nas;i++)
            {

                if((bs->apply_cashflow) != NULL)
                    _output.prices[i][j] += bs->apply_cashflow(i*ds,j*dt);


                intrate = bs->_interestrates.get_rate_with_reftime(&(bs->_interestrates),((nts-j)*dt));

                divrate = bs->_dividendrates.get_rate_with_reftime(&(bs->_dividendrates),((nts-j)*dt));

                volatility = bs->_blackscholesvol.get_vol_with_reftime(&(bs->_blackscholesvol),((nts-j)*dt),(i*ds))+increment;

                Au[i] = 1 - (intrate - divrate)*(i*ds)*dt/ds+0.5*volatility*volatility*i*ds*i*ds*dt/(ds*ds);
                Bu[i] = 0.5*volatility*volatility*i*ds*i*ds*dt/(ds*ds);
                Eu[i] = -(intrate-divrate)*i*ds*dt/ds;
                Fu[i] = 0.5*volatility*volatility*i*ds*i*ds*dt/(ds*ds);
                Du[i] = 1 - intrate*dt;
                if(i!=0 && i!=(nas-1) && j>0)
                {
                    _output.prices[i][j] = (Bu[i]*_output.prices[i-1][j]+_output.prices[i][j-1]*Du[i]+_output.prices[i-1][j-1]*Eu[i]+(_output.prices[i+1][j-1]-_output.prices[i][j-1])*Fu[i])/Au[i];
                }

                if(i==0 && j>0)
                {
                    _output.prices[i][j]=_output.prices[i][j-1]*(1-intrate*dt);
                }

                if(i==(nas-1))
                {
                    _output.prices[i][j]=2*_output.prices[i-1][j] - _output.prices[i-2][j];
                }
                 //apply boundary conditions...

                _apply_cash_flows(bs,j,&_output);

               if((bs->hedge_instruments) != NULL)
                   _hedge_instruments(bs,i*ds,j*dt,&_output);



                if((bs->apply_coupon) != NULL)
                {
                        double valueflag = bs->apply_coupon(i*ds,j*dt);
                        if(valueflag != -1) _output.prices[i][j] = valueflag;
                 }

            }

        }
        else
        {
            //downstream
            intrate = bs->_interestrates.get_rate_with_reftime(&(bs->_interestrates),((nts-j)*dt));
            divrate = bs->_dividendrates.get_rate_with_reftime(&(bs->_dividendrates),((nts-j)*dt));
            volatility = bs->_blackscholesvol.get_vol_with_reftime(&(bs->_blackscholesvol),((nts-j)*dt),(i*ds))+increment;
            for(i=nas-1;i>=0;i--)
            {
                Au[i] = 1 - (intrate-divrate)*(i*ds)*dt/ds+0.5*volatility*volatility*i*ds*i*ds*dt/(ds*ds);
                Bu[i] = 0.5*volatility*volatility*i*ds*i*ds*dt/(ds*ds);
                Eu[i] = -(intrate-divrate)*i*ds*dt/ds;
                Fu[i] = 0.5*volatility*volatility*i*ds*i*ds*dt/(ds*ds);
                Du[i] = 1 - intrate*dt;

                if(i!=(nas-1) && i!=0)
                _output.prices[i][j] = (Bu[i]*_output.prices[i+1][j]+_output.prices[i][j-1]*Du[i]+_output.prices[i-1][j-1]*Eu[i]+Fu[i]*(_output.prices[i-1][j-1]-_output.prices[i][j-1]))/Au[i];

                if(i==(nas-1))
                {
                    _output.prices[i][j]=2*_output.prices[i-1][j-1] - _output.prices[i-2][j-1];
                }


                if(i==0 && j>0)
                    _output.prices[i][j]=_output.prices[i][j-1]*(1-intrate*dt);


                _apply_cash_flows(bs,j,&_output);

                if((bs->hedge_instruments) != NULL)
                    _hedge_instruments(bs,i*ds,j*dt,&_output);

                if((bs->apply_cashflow) != NULL)
                    _output.prices[i][j] += bs->apply_cashflow(i*ds,j*dt);

	       if((bs->apply_coupon) != NULL)
		{
                      double valueflag = bs->apply_coupon(i*ds,j*dt);
		      if(valueflag != -1) _output.prices[i][j] = valueflag;
		}
            }
        }

     }


return _output;
}
//solve the black scholes PDE using an explicit scheme - rarely this will be used.
results solvebs_explicit(blackscholes1d* bs)
 {
    double dt = 0.9/(4*0.20*0.20*100*100);
    int nts = (int)(bs->expiry / dt);
    double ds = bs->stepsize;
    bs->nts = nts;
    bs->dt = dt;
    results _output;
    results _output_2;
    int i,j;
    //2D array using pointers
   _init_results(&_output,bs->numberofsteps,bs->nts);
   _init_results(&_output_2,bs->numberofsteps,bs->nts);

    //initialize prices to size nts and numberofsteps - animesh
    double intrate = 0.0;   //temp - animesh
    double volatility = 0.20; // temp - animesha
    double volatility_2 = 0.20;
    double divrate = 0.0;
    for(j=0;j<nts-1;j++)
    {

      _apply_cash_flows(bs,j,&_output);
      _apply_cash_flows(bs,j,&_output_2);
      if((bs->hedge_instruments) != NULL)
	      _hedge_instruments(bs,i*ds,j*dt,&_output);


   	  for(i=1;i<bs->numberofsteps-1;i++)
	  {
		//need to put a flag variable here
	   if((bs->apply_cashflow) != NULL)
	  {
	  	  _output.prices[i][j] += bs->apply_cashflow(i*ds,j*dt);
	         _output_2.prices[i][j] += bs->apply_cashflow(i*ds,j*dt);
	 }
	    intrate = bs->_interestrates.get_rate_with_reftime(&(bs->_interestrates),((nts-j)*dt));
	    divrate = bs->_dividendrates.get_rate_with_reftime(&(bs->_dividendrates),((nts-j)*dt));
	    volatility = bs->_blackscholesvol.get_vol_with_reftime(&(bs->_blackscholesvol),((nts-j)*dt),(i*ds));
	    //parallel bump
	    volatility_2 = volatility+0.01;
//awesome - damn awesome - animesh

	     _output.delta[i][j] = (_output.prices[i+1][j] - _output.prices[i-1][j])/(2*ds);
	    _output.gamma[i][j] = (_output.prices[i+1][j]+_output.prices[i-1][j]-2*_output.prices[i][j])/(ds*ds);
	    _output.theta[i][j] = -0.5*volatility*volatility*i*ds*i*ds*_output.gamma[i][j] -(intrate-divrate)*i*ds*_output.delta[i][j] + intrate*_output.prices[i][j];
	    _output.prices[i][j+1]=_output.prices[i][j]-dt*_output.theta[i][j];

	    _output_2.delta[i][j] = (_output_2.prices[i+1][j] - _output_2.prices[i-1][j])/(2*ds);
	    _output_2.gamma[i][j] = (_output_2.prices[i+1][j]+_output_2.prices[i-1][j]-2*_output_2.prices[i][j])/(ds*ds);
	    _output_2.theta[i][j] = -0.5*volatility_2*volatility_2*i*ds*i*ds*_output_2.gamma[i][j] -(intrate-divrate)*i*ds*_output_2.delta[i][j] + intrate*_output_2.prices[i][j];
	    _output_2.prices[i][j+1]=_output_2.prices[i][j]-dt*_output_2.theta[i][j];


	  }
 	_output.prices[0][j+1] = _output.prices[0][j]*(1-intrate*dt);
	_output.prices[bs->numberofsteps-1][j+1] = 2 *_output.prices[bs->numberofsteps-2][j+1]-_output.prices[bs->numberofsteps-3][j+1];

	_output_2.prices[0][j+1] = _output_2.prices[0][j]*(1-intrate*dt);
	_output_2.prices[bs->numberofsteps-1][j+1] = 2 *_output_2.prices[bs->numberofsteps-2][j+1]-_output_2.prices[bs->numberofsteps-3][j+1];

     }

//populate vega
	for(j=0;j<nts;j++)
		for(i=0;i<bs->numberofsteps;i++)
			_output.vega[i][j] = (_output.prices[i][j] - _output_2.prices[i][j]);

return _output;
}

//solve blackscholes PDE using implicit scheme - use of GNU Scientific library
//Implcit schemes will always be preferred for 1D
results solvebs(blackscholes1d* bs)
{

	double inc = 0.0;
	results _output = solve_experimental(bs,inc);


	results _output_2 = solve_experimental(bs,inc+0.01);

	int i,j;
	for(j=0;j<bs->nts;j++)
	{
		for(i=0;i<bs->numberofsteps-1;i++)
		{
			_output.vega[i][j] = _output.prices[i][j] - _output_2.prices[i][j];
			if(i > 1)
			{
				_output.delta[i][j] = (_output.prices[i+1][j]-_output.prices[i-1][j])/(2*bs->stepsize);
				_output.gamma[i][j] = (_output.prices[i+1][j] + _output.prices[i-1][j] - 2*_output.prices[i][j])/(bs->stepsize*bs->stepsize);
			}
			if(j > 1)
				_output.theta[i][j] = (_output.prices[i][j-1] - _output.prices[i][j])/(bs->dt);
		}
	}

	//results _output_2 = solvebs_implicit(bs,0.01);
	//compute vega here - animesh
	return _output;
}

//always use implicit schemes - this is the default setting for 1D model
results solvebs_implicit(blackscholes1d* bs,double increment)
{

    double dt = bs->dt;
    int nts = (int)(bs->expiry / dt);
    double ds = bs->stepsize;
    bs->nts = nts;
    bs->dt = dt;
    results _output;
    int i,j;
   _init_results(&_output,bs->numberofsteps,bs->nts);

	    double intrate = 0.0;   //temp - animesh
	    double volatility = 0.0; // temp - animesh
	    double divrate = 0.0;

	gsl_vector *x = gsl_vector_alloc(bs->numberofsteps);

    int s;
	double *A = (double*)malloc(sizeof(double)*bs->numberofsteps);
	double *B = (double*)malloc(sizeof(double)*bs->numberofsteps);
	double *C = (double*)malloc(sizeof(double)*bs->numberofsteps);
	double *coeffs = (double*)malloc(sizeof(double)*(bs->numberofsteps)*(bs->numberofsteps));
	double* tmp = (double*)malloc(sizeof(double)*(bs->numberofsteps));

    for(j=0;j<nts-1;j++)
    {
        _apply_cash_flows(bs,j,&_output);

	for(i=0;i<(bs->numberofsteps-1);i++)
	{
         if((bs->hedge_instruments) != NULL)
	      _hedge_instruments(bs,i*ds,j*dt,&_output);

	  if((bs->apply_cashflow) != NULL)
		  {
		  	  _output.prices[i][j] += bs->apply_cashflow(i*ds,j*dt);
		  }

	    intrate = bs->_interestrates.get_rate_with_reftime(&(bs->_interestrates),((nts-j)*dt));
	    divrate = bs->_dividendrates.get_rate_with_reftime(&(bs->_dividendrates),((nts-j)*dt));
	    volatility = bs->_blackscholesvol.get_vol_with_reftime(&(bs->_blackscholesvol),((nts-j)*dt),(i*ds))+increment;


		A[i] = (dt/(ds*ds))*0.5*volatility*volatility*((i+1)*ds*(i+1)*ds);
		B[i] =  (intrate-divrate)*(i+1)*ds*dt/(2*ds);
		C[i] = intrate * dt;
	}

	long stepsquared = bs->numberofsteps *bs->numberofsteps;
	long p1;
	for(p1=0;p1<stepsquared;p1++)
		coeffs[p1]=0;
	coeffs[0] = (1-intrate*dt);

	//applying the condition V(NAS) = 2*V(NAS-1) - V(NAS-2)
	coeffs[stepsquared-3] = -1;
	coeffs[stepsquared-2] = 2;
	coeffs[stepsquared-1] = -1;
	for(i=1;i<bs->numberofsteps-1;i++)
	{
		coeffs[i+bs->numberofsteps*i-1]=-A[i]+B[i];
		coeffs[i+bs->numberofsteps*i]=1+2*A[i]-C[i];
		coeffs[i+bs->numberofsteps*i+1]=-1*A[i]-B[i];
	}

	_output.prices[bs->numberofsteps-1][j] = 0;

	gsl_matrix_view m = gsl_matrix_view_array(coeffs,bs->numberofsteps,bs->numberofsteps);
	for(p1=0;p1<bs->numberofsteps;p1++)
		tmp[p1]=_output.prices[p1][j];


	gsl_vector_view b = gsl_vector_view_array(tmp,bs->numberofsteps);
	gsl_permutation *p = gsl_permutation_alloc(bs->numberofsteps);


	gsl_linalg_LU_decomp(&m.matrix,p,&s);
	gsl_linalg_LU_solve(&m.matrix,p,&b.vector,x);

	for(p1=0;p1<bs->numberofsteps;p1++)
		_output.prices[p1][j+1] = x->data[p1];
    }

return _output;
}

//Rough Notes - Animesh
//how to link the model chosen with bounday conditions
//Common points in boundary condition --
//1. Each boundary condition is applied at a specific time
//2. It has a specific payoff so why not keep it in any array
//3. This array should syncrhonize with the time step and space step chosen
//4. This should complete our generic model of the model and FD Engine / Calculator! Any loopholes???
//5.

