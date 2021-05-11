#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


#define CLIENTES 10
#define CADEIRAS 5
int clientes_esperando =0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
sem_t barbeiros;
sem_t clientes;

void delay(int secs)
{ /*utility function*/
    time_t beg = time(NULL), end = beg + secs;
    do
    {
        ;
    } while (time(NULL) < end);
}
void *barbeiro (void *i){
    while(1){
        sem_wait(&clientes);//se não tiver nenhum cliente ele dorme
        pthread_mutex_lock(&mutex);// travando a região crítica
        clientes_esperando --;
        sem_post(&barbeiros);//barbeiro pronto para cortar o cabelo
        pthread_mutex_unlock(&mutex);//destravando região crítica
        printf("Barbeiro cortou cabelo de um cliente\n");
    }
    pthread_exit( (void*) 0 );
}

void * cliente(void*i){
     
    int num_cliente = *(int*)i;
    pthread_mutex_lock(&mutex);
    if(clientes_esperando<CADEIRAS){
        clientes_esperando ++;
        printf("Cliente %d entrou no barbeiro\n",num_cliente);
        sem_post(&clientes);//sinalizando pode cortar o cabelo
        printf("Cliente %d sentou na cadeira do barbeiro \n",num_cliente);
        pthread_mutex_unlock(&mutex);
        delay(rand() % 6);
        printf("Cliente %d saiu da barbearia \n",num_cliente);
    }
    else{
        pthread_mutex_unlock(&mutex);
        printf("Cliente %d não entrou, pois estava cheio\n",num_cliente);
    }
}
int main(){
    pthread_t thrd_clientes[CLIENTES], thrd_barbeiro;
    sem_init(&clientes,0,0);
    sem_init(&barbeiros,0,0);
    int i;
    for (i=0;i<CLIENTES;i++){
        int *arg = malloc(sizeof(*arg));
        if (arg == NULL) {
            fprintf(stderr, "Não foi possível alocar o arg\n");
            exit(EXIT_FAILURE);
        }
        *arg = i;
        pthread_create(&thrd_clientes[i],NULL,cliente,arg);
        delay(rand() % 6);
    }
    pthread_create(&thrd_barbeiro,NULL,barbeiro,NULL);
    for (i=0;i<CLIENTES;i++){
        pthread_join(thrd_clientes[i], NULL);
    }
    pthread_exit(NULL);
    return 0;
}
