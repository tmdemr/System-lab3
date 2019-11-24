#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>
#include<mqueue.h>
#include<pthread.h>
#include<time.h>

#define MQ_1 "/mq1"
#define MQ_2 "/mq2"
#define MSG_SIZE 256
#define MAX_MSG 5
#define MAX_LOG 512

#define SEND "P1"
#define RECV "P2"

pthread_mutex_t mutex;
int fd;
mqd_t mq1, mq2;
char send_buf[MSG_SIZE];
char recv_buf[MSG_SIZE];
char log_buf[MAX_LOG];
char *ptr = '\0';
time_t ltime;
struct tm *today;
short in_user = 0;

void logg_f(char* str, char* user);
void *send_thread(void* args);
void *recv_thread(void*args);

int main(int argc,char** argv) {
   struct mq_attr attr;
   pthread_t sendT,recvT;
   int ret, status=0;
   char namebuf[10];
   sprintf(namebuf,"%d.txt",getpid());

   pthread_mutex_init(&mutex,NULL);
   attr.mq_maxmsg=MAX_MSG;
   attr.mq_msgsize=MSG_SIZE;

   mq1 = mq_open(MQ_1,O_CREAT|O_RDWR,0666,&attr);
   mq2 = mq_open(MQ_2,O_CREAT|O_RDWR,0666,&attr);
   fd = open(namebuf,O_WRONLY|O_CREAT,0666);

   if((mq1==(mqd_t)-1)||(mq2==(mqd_t)-1)) {
      perror("open message queue error");
      exit(0);
   }

   if(fd==-1) {
      printf("cannot open file %s\n", namebuf);
      exit(0);
   }

   ret = pthread_create(&sendT,NULL,send_thread,(void*)&mq1);
   if(ret<0) {
      perror("thread create error:");
      exit(0);
   }

   ret = pthread_create(&recvT,NULL,recv_thread,(void*)&mq2);
   if(ret<0) {
      perror("thread create error:");
      exit(0);
   }

   printf("### Join chatt!! ###\n");
   strcpy(send_buf,"/s");
   mq_send(mq1,send_buf,strlen(send_buf),0);

   pthread_join(sendT,(void**)&status);
   pthread_join(recvT,(void**)&status);

   return 0;
}

void logg_f(char* str, char* user) {
   pthread_mutex_lock(&mutex);
   time(&ltime);
   today = localtime(&ltime);
   ptr = asctime(today);
   ptr[strlen(ptr)-1]='\0';
   sprintf(log_buf,"[%s]%s:%s\n", ptr,user,str);
   write(fd, log_buf, strlen(log_buf));
   memset(log_buf, '\0', sizeof(log_buf));
   pthread_mutex_unlock(&mutex);
}

void *send_thread(void* args) {
   while(1) {
      memset(send_buf,'\0',sizeof(send_buf));
      printf(">");
      fgets(send_buf,sizeof(send_buf),stdin);
      send_buf[strlen(send_buf)-1]='\0';

      if(mq_send(*(mqd_t*)args,send_buf,strlen(send_buf),0)==-1) {
         perror("mq_send()");
      }
      else {
         logg_f(send_buf,SEND);
         if(strcmp(send_buf,"/q")==0) {
            printf("### Finish Chat ###\n");
            pthread_mutex_destroy(&mutex);
            close(fd);
            mq_close(mq1);
            mq_close(mq2);
            if(in_user){
               mq_unlink(MQ_1);
               mq_unlink(MQ_2);
            }
            exit(0);
         }
         printf("%s:%s\n",SEND,send_buf);
      }
   }
}


void *recv_thread(void*args) {
   while(1) {
      while(mq_receive(*(mqd_t*)args,recv_buf,sizeof(recv_buf),0)>0) {
         logg_f(recv_buf,RECV);
         if(strcmp(recv_buf,"/s")==0) {
            in_user=0;
            memset(recv_buf,'\0',sizeof(recv_buf));
            break;
         }

         if(strcmp(recv_buf,"/q")==0) {
            in_user=1;
            printf("Another user is out of here.... \n");
            memset(recv_buf,'\0',sizeof(recv_buf));
            break;
         }
         printf("%s:%s\n",RECV,recv_buf);
         memset(recv_buf,'\0',sizeof(recv_buf));
      }
   }
}

