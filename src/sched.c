#include "thread.h"
#include <stdlib.h>
#include <sys/time.h>
#include <signal.h>
#include <stdio.h>
#include <Windows.h>

extern struct task_struct *current;
extern struct task_struct *task[NR_TASKS];

void switch_to(struct task_struct *next);

static unsigned long long getmstime() {
  struct timeval tv;
  if (gettimeofday(&tv, NULL) < 0) {
    perror("gettimeofday");
    exit(-1);
  }
  return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

static struct task_struct *pick() {

/*
找到时间片最大的线程进行调度
*/

  
  


  
  int i, next, c;



  for (i = 0; i < NR_TASKS; ++i) {

    if(!task[i])continue;

    if( task[i]->status == THREAD_EXIT){
      if(task[i]!=current)
        __remove_thread__(i);
      continue;
    }
  
    if (task[i]->status == THREAD_DISPOSED)
   {
     if(task[i]!=current)
      __remove_thread__(i);
     continue;
   }

    if (task[i]->status != THREAD_STOP&& task[i]->status != THREAD_BLOCK
        && getmstime() > task[i]->wakeuptime) {
      task[i]->status = THREAD_RUNNING;
    }

  }

  //上面的作用是唤醒睡眠的线程,使其可以接受调度

  while(1) {
    c = -1;
    next = 0;
    for (i = 0; i < NR_TASKS; ++i) {
      if (!task[i]) continue;
      if (task[i]->status == THREAD_RUNNING && task[i]->counter > c) {
        c = task[i]->counter;
        next = i;
      }
    }
    if (c) break;

    // 如果所有任务时间片都是 0，重新调整时间片的值
    if (c == 0) {
      for (i = 0; i < NR_TASKS; ++i) {
        if(task[i]) {
          task[i]->counter = task[i]->priority + (task[i]->counter >> 1);
        }
      }
    }
  }
  
  return task[next];
}

volatile int signal_blocked = 0;

void closealarm() {

    signal_blocked = 1;
}

void openalarm() {

    signal_blocked = 0;
}


void schedule() {
    struct task_struct *next = pick();
    if (next) {
      switch_to(next);
    }
}

void mysleep(int milliseconds) {
  current->wakeuptime = getmstime() + milliseconds;
  current->status = THREAD_SLEEP;
  schedule();
}

void WINAPI do_timer(UINT uTimerID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2) {
  if (signal_blocked) return;
  if (--current->counter > 0) return;
  current->counter = 0;
  schedule();
  //printf("do_timer\n");
}

__attribute__((constructor))
static void init() {

  timeSetEvent(1000, 1, (LPTIMECALLBACK)do_timer, (DWORD_PTR)NULL, TIME_PERIODIC);
}