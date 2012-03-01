/*
 Copyright (C) 2012 Animesh Saxena

 This file is part of TradeAnt, a free-software/open-source library
 for traders and practical quantitative analysts

 TradeAnt is free software: you can redistribute it. Use it at your own risk
*/


#include "dates.h"
#include <string.h>

void setreftime(int Year,int months,int day)
{
	 extern struct tm _globalref;
 	 _globalref.tm_year = Year - 1900;
    	_globalref.tm_mon = months;
	_globalref.tm_mday = day;
}
void _initref(qdate *q,int Year,months m,int day)
{
    q->ref_time.tm_year=Year-1900;
    q->ref_time.tm_mon = m;
    q->ref_time.tm_mday = day;
    q->ref_time.tm_hour = 0;
    q->ref_time.tm_min = 0;
    q->ref_time.tm_sec = 0;
    q->ref_time.tm_isdst = 0;
}
void _initdate(qdate *q,int Year,months m,int day)
  {

    q->str_time.tm_year=Year-1900;
    q->str_time.tm_mon = m;
    q->str_time.tm_mday = day;
    q->str_time.tm_hour = 0;
    q->str_time.tm_min = 0;
    q->str_time.tm_sec = 0;
    q->str_time.tm_isdst = 0;

    extern struct tm _globalref;
    q->ref_time = _globalref;

    time_t t1 = mktime(&(q->str_time));
    time_t t2 = mktime(&(q->ref_time));
    q->difference = difftime(t1,t2)/(86400*365);

 }
void _initstringdate(qdate *q,char *a)
  {
    char s[25];
	strcpy(s,a);
//a	printf("%s",s);
//	exit(0);
    int d = atoi(strtok(s,"\/"));
    int m = atoi(strtok(NULL,"\/"));
    int year = 2000+atoi(strtok(NULL,"\/"));
 //   printf("%d %d %D",d,m,year);
//	exit(0);
    q->str_time.tm_year=year-1900;
    q->str_time.tm_mon = m;
    q->str_time.tm_mday = d;
    q->str_time.tm_hour = 0;
    q->str_time.tm_min = 0;
    q->str_time.tm_sec = 0;
    q->str_time.tm_isdst = 0;

    extern struct tm _globalref;
    q->ref_time = _globalref;

    time_t t1 = mktime(&(q->str_time));
    time_t t2 = mktime(&(q->ref_time));
    q->difference = difftime(t1,t2)/(86400*365);

  }

void initqdate(qdate *m)
{
	m->initdate = _initdate;
	m->initstringdate = _initstringdate;
	m->initrefdate = _initref;

}
