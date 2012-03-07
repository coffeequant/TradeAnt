/*
 Copyright (C) 2012 Animesh Saxena

 This file is part of TradeAnt, a free-software/open-source library
 for traders and practical quantitative analysts

 TradeAnt is free software: you can redistribute it. Use it at your own risk
*/


#include <tradeant/boundary.h>

void initialize_cashflows1d(cashflows1d* bc,int n)
{
	bc->time = (double*)malloc(sizeof(double)*n);
	bc->value = (double*)malloc(sizeof(double)*n);
	bc->barrier =(double*)malloc(sizeof(double)*n);
	bc->bccount = n;
}
