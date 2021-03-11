#include "wgt.h"
#include "include/mb.h"

// ######## REQUIRED FUNCTIONS ########

unsigned long state0 = 1000;
unsigned long state1 = 2000;

unsigned long rand(void)
{
    unsigned long s1 = state0;
    unsigned long s0 = state1;

    state0 = s0;
    s1 ^= s1 << 23;
    s1 ^= s1 >> 17;
    s1 ^= s0;
    s1 ^= s0 >> 26;
    state1 = s1;

    return state0 + state1;
}

void wait_msec(unsigned int n)
{
    register unsigned long f, t, r;

    // Get the current counter frequency
    asm volatile ("mrs %0, cntfrq_el0" : "=r"(f));
    // Read the current counter
    asm volatile ("mrs %0, cntpct_el0" : "=r"(t));
    // Calculate expire value for counter
    t+=((f/1000)*n)/1000;
    do{asm volatile ("mrs %0, cntpct_el0" : "=r"(r));}while(r<t);
}

// ######## STUB FUNCTIONS ########

unsigned int kb = 0;

unsigned int kbhit(void) {
    kb++;
    return kb / 500;
}

void getch(void) {
    wait_msec(0x500000);
    kb = 0;
}

// ######## WGT EXAMPLES ########

#define TIMERSPEED 60     /* timer_routine is called 60 times per second */

int timer;                /* Counts how many times it has been called */ 
int clearcount;           /* Counts the number of screen clears. */

extern int get_el(void);

void timer_routine (void)
{
  timer++;
}

void wgt03()
{
  vga256 ();                     /* Initialize WGT system        */

  debugstr ("WGT Example #3"); debugcrlf(); debugcrlf(); 
  debugstr ("This program will use the wcls routine to clear the screen"); debugcrlf();
  debugstr ("using random colors as fast as it can until you press a key."); debugcrlf();
  debugstr ("It will then report the highest frame rate possible on your computer."); debugcrlf(); debugcrlf(); debugcrlf();

  int el = get_el();
  debugstr("Exception level: "); debughex(el); debugcrlf();
  int setter = set_clock_rate(get_max_clock());
  debugstr("Set clock returned "); debughex(setter); debugcrlf(); debugcrlf(); debugcrlf();

  debugstr ("Press any key to continue.");
  getch ();

  clearcount = 0;
  timer = 0;

  winittimer ();
  wstarttimer (timer_routine, TIMERSPEED);

  debugreset();

  while (!kbhit ())
  {
     wcls(vgapal[rand() % 256]); /* Clear with random color out of 256 */
     clearcount++;
  }

  wstoptimer ();
  wdonetimer ();

  getch ();

  wcls(vgapal[0]);

  int clock = get_clock_rate();
  debugstr("Clock rate: "); debughex(clock); debugcrlf();

  int maxclock = get_max_clock();
  debugstr("Max clock rate: "); debughex(maxclock); debugcrlf();

  unsigned int fps = clearcount / (timer / TIMERSPEED);
  debughex(fps); debugstr("frames per second"); debugcrlf(); debugcrlf(); debugcrlf();

  debugstr ("This is the highest frame rate your computer can produce for full screen\n"); debugcrlf();
  debugstr ("animation.");
}

void main()
{
    wgt03();
    while (1);
}
