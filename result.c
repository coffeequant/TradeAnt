#include <tradeant/result.h>

void _init_results2d(results2d* _output,int numberofsteps,int nts)
{
   int i;
        _output->prices = (double*)malloc(sizeof(double)*numberofsteps*numberofsteps*nts);
        _output->delta= (double*)malloc(sizeof(double)*numberofsteps*numberofsteps*nts);
        _output->theta = (double*)malloc(sizeof(double)*numberofsteps*numberofsteps*nts);
        _output->gamma = (double*)malloc(sizeof(double)*numberofsteps*numberofsteps*nts);
        _output->vega = (double*)malloc(sizeof(double)*numberofsteps*numberofsteps*nts);

        _output->i = numberofsteps;
        _output->j = numberofsteps;
        _output->k = nts;

        _output->get_prices=_get_prices;
        _output->get_delta=_get_delta;
        _output->get_theta=_get_theta;
        _output->get_vega=_get_vega;
        _output->get_gamma=_get_gamma;
}

double _get_prices(struct _results2d* rs,int i,int j,int k)
{
    return _get_array(1,rs,i,j,k);
}

double _get_delta(struct _results2d* rs,int i,int j,int k)
{
    return _get_array(2,rs,i,j,k);
}

double _get_gamma(struct _results2d* rs,int i,int j,int k)
{
    return _get_array(3,rs,i,j,k);
}

double _get_theta(struct _results2d* rs,int i,int j,int k)
{
    return _get_array(4,rs,i,j,k);
}

double _get_vega(struct _results2d* rs,int i,int j,int k)
{
    return _get_array(5,rs,i,j,k);
}

double _get_array(int con,struct _results2d* rs,int i,int j,int k)
{
    if(con == 1)
        return rs->prices[k*rs->i*rs->j+j*rs->j+i];
    if(con == 2)
        return rs->delta[k*rs->i*rs->j+j*rs->j+i];
    if(con == 3)
        return rs->gamma[k*rs->i*rs->j+j*rs->j+i];
    if(con == 4)
        return rs->theta[k*rs->i*rs->j+j*rs->j+i];
    if(con == 5)
        return rs->vega[k*rs->i*rs->j+j*rs->j+i];


}

void _init_results(results *_output,int numberofsteps,int nts)
{
   int i;
   _output->prices = (double**)malloc(sizeof(double*)*(numberofsteps));
   _output->delta = (double**)malloc(sizeof(double*)*(numberofsteps));
   _output->gamma = (double**)malloc(sizeof(double*)*(numberofsteps));
   _output->theta = (double**)malloc(sizeof(double*)*(numberofsteps));
   _output->vega = (double**)malloc(sizeof(double*)*(numberofsteps));

    for(i=0;i<numberofsteps;i++) {
        _output->prices[i] = (double*)malloc(sizeof(double)*nts);
        _output->delta[i] = (double*)malloc(sizeof(double)*nts);
        _output->theta[i] = (double*)malloc(sizeof(double)*nts);
        _output->gamma[i] = (double*)malloc(sizeof(double)*nts);
        _output->vega[i] = (double*)malloc(sizeof(double)*nts);
     }
};


