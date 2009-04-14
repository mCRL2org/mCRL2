#ifndef TIMING_H_INCLUDED
#define TIMING_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

void time_initialize();     /* initialize the timer */
void time_pause();          /* pause the timer */
double time_resume();       /* resume the timer; reports time waited */
double time_used();         /* number of seconds used */

#ifdef __cplusplus
}
#endif

#endif /* TIMING_H_INCLUDED */
