#include "thread.h"
#include <stdlib.h>
#include <signal.h>
#include <sys/time.h>
#include <stdio.h>



void schedule();



static struct task_struct init_task = { 
    0, 
    -1, 
    NULL, 
    0, 
    0, 
    THREAD_RUNNING, 
    15, 
    15, 
    {0},
    {
        {0},
        0,
        0
    }
};

struct task_struct* current = &init_task;

struct task_struct* task[NR_TASKS] = { &init_task, };

// 线程启动函数
static void start(struct task_struct* tsk)
{
    tsk->th_fn();
    tsk->status = THREAD_EXIT;
    printf("thread [%d] exited\n", tsk->id);
    schedule();
    // 下面这一行永远不会被执行
    printf("thread [%d] resume\n", tsk->id);
}




int create_thread(int* tid, void (*start_routine)())
{
    int id = -1;
    struct task_struct* tsk = (struct task_struct*) malloc(sizeof(struct task_struct));
    while (++id < NR_TASKS && task[id]);
    if (id == NR_TASKS) return -1;
    task[id] = tsk;
    if (tid) *tid = id;  //返回值
    tsk->id = id;
    tsk->parent_id = current->id;
    tsk->th_fn = start_routine;
    long long* stack = tsk->stack; // 栈顶界限
    tsk->esp = (long long) (stack + STACK_SIZE - 11);
    tsk->wakeuptime = 0;
    tsk->status = THREAD_STOP;
    tsk->counter = 15;
    tsk->priority = 15;
    // 初始 switch_to 函数栈帧
    stack[STACK_SIZE - 11] = 0; // flags
    stack[STACK_SIZE - 10] = 0; // rax
    stack[STACK_SIZE - 9] = (long long) tsk; // rcx 经观察汇编后发现,start的参数(struct task_struct *tsk)实际上由rcx取得.
    stack[STACK_SIZE - 8] = 0; // rdx
    stack[STACK_SIZE - 7] = 0; // rbx
    stack[STACK_SIZE - 6] = 0; // rsi
    stack[STACK_SIZE - 5] = 0; // rdi
    stack[STACK_SIZE - 4] = 0; // old rbp  
    stack[STACK_SIZE - 3] = (long long) start; // ret to start   线程第一次被调度时会在此启动
    // start 函数栈帧，刚进入 start 函数的样子 
    stack[STACK_SIZE - 2] = 100;// ret to unknown，如果 start 执行结束，表明线程结束 
    stack[STACK_SIZE - 1] = (long long) tsk; // start 的参数
    
    tsk->message_queue.head = 0;
    tsk->message_queue.rear = 0;

    /*
    汇编函数调用,c风格参数传递
    传入参数分别是IP,c1,c2
    */
    return 0;
}


void block(int tid)
{
    if (task[tid]->parent_id != current->id) {
        printf("\nError: only parent thread can operate it.\n");
        return;
    }

    if (task[tid] != NULL && task[tid]->status != THREAD_STOP && task[tid]->status != THREAD_EXIT) {
        task[tid]->status = THREAD_BLOCK;
    }

}

void resume(int tid)
{
    if (task[tid]->parent_id != current->id) {
        printf("\nError: only parent thread can operate it.\n");
        return;
    }
    if (task[tid] != NULL && task[tid]->status != THREAD_STOP && task[tid]->status != THREAD_EXIT && task[tid]->status == THREAD_BLOCK) {
        task[tid]->status = THREAD_RUNNING;
    }
}

void detach(int tid)
{
    if (task[tid]->parent_id != current->id) {
        printf("\nError: only parent thread can operate it.\n");
        return;
    }
    if (task[tid] != NULL && task[tid]->status == THREAD_STOP && task[tid]->status != THREAD_EXIT) {
        task[tid]->status = THREAD_RUNNING;

        schedule();
    }

}


void wait_all()
{

    if (current->id != 0) {
        printf("\nError: only main thread can operate it.\n");
        return;
    }

    int i = 0;
    int remain = 0;
    while (1) {
        remain = 0;
        for (i = 1;i < NR_TASKS;i++) {
            if (task[i] && task[i]->status != THREAD_EXIT) {
                remain = 1;
                schedule();
                break;
                continue;
            }
        }
        if (!remain) {
            break;
        }


    }

}

void wait(int tid)
{
    while (task[tid] && task[tid]->status != THREAD_EXIT)
    {
        schedule();
    }
}

void __remove_thread__(int tid)
{
    if (task[tid]) {

        free(task[tid]);  //释放空间
        task[tid] = NULL;
        printf("the resource of thread %d is released", tid);
    }
}

void dispose(int tid)
{
    if (task[tid]->parent_id != current->id) {
        printf("\nError: only parent thread can operate it.\n");
        return;
    }

    if (task[tid] != NULL) {

        task[tid]->status = THREAD_DISPOSED;

    }

}


int thread_join(int tid)
{
    while (task[tid] && task[tid]->status != THREAD_EXIT) {
        if (task[tid]->status == THREAD_STOP) {
            task[tid]->status = THREAD_RUNNING;
        }
        schedule();
    }

}

void send_to(int tid, void* p_msg) {
    if(!task[tid]) {
        printf("Error: inexist task of tid %d", tid);
        return;
    }
    struct message_queue* t_queue = &task[tid]->message_queue;
    t_queue->data[t_queue->rear++] = p_msg;
    t_queue->rear %= M_QUEUE_SIZE;
}

void* receive_a_message() {
    while (current->message_queue.head == current->message_queue.rear) {
        schedule();
    }
    struct message_queue* t_queue = &current->message_queue;
    void* res = t_queue->data[t_queue->head++];
    t_queue->head %= M_QUEUE_SIZE;
    return res;
}