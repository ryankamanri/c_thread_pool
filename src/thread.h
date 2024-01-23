#ifndef __THREAD_H__
#define __THREAD_H__

#define NR_TASKS 1024
#define STACK_SIZE 1024 // 1024*4 B
#define M_QUEUE_SIZE 1024

#define THREAD_READY 0
#define THREAD_RUNNING 1
#define THREAD_SLEEP 2
#define THREAD_BLOCK 3
#define THREAD_EXIT 4
#define THREAD_STOP 5
#define THREAD_DISPOSED 6

struct task_struct {

  int id;  //线程的标识符
  int parent_id; // 父线程id
  void (*th_fn)(); //指向线程函数的函数指针
  int esp; //用来在发生线程切换是保存线程的栈顶地址
  unsigned long long wakeuptime; // 线程唤醒时间
  int status; // 线程状态
  int counter; // 时间片
  int priority; // 线程优先级
  long long stack[STACK_SIZE]; //现场的栈空间
  struct message_queue {
    void* data[M_QUEUE_SIZE];
    int head;
    int rear;
  } message_queue;  // 消息队列
};

void wait(int tid); // BLOCKABLE
int create_thread(int *tid, void (*start_routine)());
int thread_join(int tid); // BLOCKABLE
void resume(int tid);
void block(int tid);
void mysleep(int milliseconds);
void detach(int tid);
void __remove_thread__(int tid);
void dispose(int tid);

void send_to(int tid, void* p_msg);
void* receive_a_message(); // BLOCKABLE

#endif //__THREAD_H__