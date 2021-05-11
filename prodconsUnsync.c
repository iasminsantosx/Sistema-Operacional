#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

/* buffer using a shared integer variable */
typedef struct {
int writeable; /*true/false*/
int sharedData;
} buffer;

buffer theBuffer; /* global */
pthread_mutex_t mutex= PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condProd = PTHREAD_COND_INITIALIZER;
pthread_cond_t condCons = PTHREAD_COND_INITIALIZER;

int store(int item, buffer *pb) {
int num;
pthread_mutex_lock(&mutex);
if (pb->writeable==0){
num=0;
printf("Buffer cheio\n");
pthread_cond_wait(&condProd,&mutex);
}
else{
num=1;
pb->sharedData += item; /*put data in buffer... */
printf("Stored: %d\n", pb->sharedData);
pb->writeable = !pb->writeable;
//printf("%d\n",pb->writeable);
}
pthread_cond_signal(&condCons);
pthread_mutex_unlock(&mutex);
return num;
}

int retrieve(buffer *pb) {
int data;
pthread_mutex_lock(&mutex);
if (pb->writeable==1){
printf("Buffer vazio\n");
pthread_cond_wait(&condCons,&mutex);
}
else{
data = pb->sharedData;
//printf("Antes de consumir %d\n",pb->writeable);
pb->sharedData -= data; /*get data from buffer...*/
printf("Retrieved: %d\n", data);
pb->writeable = !pb->writeable;
//printf("Depois de consumir %d\n",pb->writeable);
}
pthread_cond_signal(&condProd);
pthread_mutex_unlock(&mutex);
return data;
}

void delay(int secs) { /*utility function*/
time_t beg = time(NULL), end = beg + secs;
do {
;
} while (time(NULL) < end);
}

/* core routine for the producer thread */
void *producer(void *n) {
int j=1;
int num;
while (j<=10) {

num=store(j, &theBuffer);
if (num==1) j++;
delay(rand() % 6); /* up to 5 sec */
}
//pthread_exit((void *)0);
return n;
}

/* core routine for the consumer thread */
void *consumer(void *n) {
int j=0, tot=0;
while (j < 10) {
j = retrieve(&theBuffer);
tot += j;
//printf("Laço %d, o tot será %d\n",j,tot);
delay(rand() % 6); /* up to 5 sec */
}
printf("Retrieved total = %d\n", tot);
return(n);
}

int main() {
pthread_attr_t *attr = NULL;
pthread_t prodThread, consThread;

theBuffer.writeable = 1;
srand(time(NULL)); /* initialise the rng */

pthread_create(&prodThread, attr, producer, NULL);
pthread_create(&consThread, attr, consumer, NULL);

pthread_join(prodThread,NULL);
pthread_join(consThread,NULL);
pthread_exit(&prodThread);
pthread_exit(&consThread);
pthread_exit(NULL);
/*loop forever*/
return 0; /*!!!*/
}