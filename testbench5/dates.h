#include <time.h>
#ifndef _dates_
#define _dates_
struct tm _globalref;

typedef enum _months {Jan,Feb,Mar,Apr,May,Jun,Jul,Aug,Sep,Oct,Nov,Dec} months;
//date format will be like date(year,month,day)
typedef struct _qdate{
  struct tm str_time;
  struct tm ref_time;
  double difference;
  //keep function pointers instead of functions
  void (*initdate)(struct _qdate*,int,months,int);
  void (*initstringdate)(struct _qdate*,char*);
  void (*initrefdate)(struct _qdate*,int,months,int);
}qdate;

void _initdate(qdate*,int,months,int);
void _initstringdate(qdate*,char*);
void _initref(qdate*,int,months,int);
void initqdate(qdate*);
#endif

