#include"thread.h"
#include<stdio.h>


void fun1(){

  int i=0;
  for(i=0;i<10;i++){
    printf("thread id=%d time=%d\n",1,i);
    mysleep(100);
  }
  char* msg = receive_a_message();
  printf("\nthread %d Received a message: %s\n", 1, msg);
}

void fun2(){


int i=0;
  for(i=0;i<10;i++){
    printf("thread id=%d time=%d\n",2,i);
    mysleep(150);
  }


}

const char* th3_msg = "Love from thread 3!!!";
void fun3(){

  int i=0;
  for(i=0;i<10;i++){
    printf("thread id=%d time=%d\n",3,i);
    mysleep(140);
  }
  send_to(1, (void*)th3_msg);

}

void fun4() {
  int i=0;
  for(i=0;i<10;i++){
    printf("thread id=%d time=%d\n",4,i);
    mysleep(90);
  }
}


int main(){


  int th1,th2,th3,th4;
  create_thread(&th1,fun1);
  create_thread(&th2,fun2);
  create_thread(&th3,fun3);
  create_thread(&th4,fun4);
  detach(th2);
  detach(th1);
  detach(th4);
  block(2);
  thread_join(th3);
  resume(2);
  wait(1);

  return 0;
}


