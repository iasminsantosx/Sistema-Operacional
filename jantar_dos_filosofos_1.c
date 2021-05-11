// Alunas : Iasmin Santos e Myrlla Lucas
#include  <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define QUANTIDADE_FILOSOFOS 5
#define ESQUERDA (num_filosofo + QUANTIDADE_FILOSOFOS -1)%QUANTIDADE_FILOSOFOS
#define DIREITA (num_filosofo+1)%QUANTIDADE_FILOSOFOS
#define PENSANDO 0
#define FAMINTO 1
#define  COMENDO 2

int estado[QUANTIDADE_FILOSOFOS];
int variavelglobal;

pthread_mutex_t mutex;
pthread_mutex_t mutex_filosofos[QUANTIDADE_FILOSOFOS];

void teste( int num_filosofo){
    if(estado[num_filosofo]==FAMINTO && estado[ESQUERDA]!=COMENDO && estado[DIREITA]!=COMENDO){
        printf("Filosofo %d ganhou a vez para comer\n",num_filosofo);
        estado[num_filosofo] = COMENDO;
        pthread_mutex_unlock(&(mutex_filosofos[num_filosofo]));
    }

}
void delay(int secs)
{ /*utility function*/
    time_t beg = time(NULL), end = beg + secs;
    do
    {
        ;
    } while (time(NULL) < end);
}
void pensar (int num_filosofo){
    //estado[num_filosofo]= PENSANDO;
    //printf("Filosófo %d está pensando\n",num_filosofo);
    delay(rand() % 6);
}
void comer (int num_filosofo){
    //estado[num_filosofo] = COMENDO;
    //printf("Filosófo %d está comendo\n",num_filosofo);
    delay(rand() % 6);
}
void pegar_talher(int num_filosofo){
    int pegou_tlher;
    pthread_mutex_lock(&mutex);
    estado[num_filosofo]= FAMINTO;
    teste(num_filosofo);
    pthread_mutex_unlock(&mutex);
    pthread_mutex_lock(&mutex_filosofos[num_filosofo]);
}
void devolver_talher(int num_filosofo){
    pthread_mutex_lock(&mutex);
    estado[num_filosofo]= PENSANDO;
    teste(ESQUERDA);
    teste(DIREITA);
    printf("Filosofo %d está devolvendo o talher\n",num_filosofo);
    pthread_mutex_unlock(&mutex);
}
void *filosofo ( void * num_filosofo){
    int identificador = variavelglobal++;
    printf("Filosofo %d criado\n",identificador);
    while (1){
        pensar(identificador);
        pegar_talher(identificador);
        comer(identificador);
        devolver_talher(identificador);
    }

    pthread_exit( (void*) 0 );
}
int main() {
    pthread_t jantar[QUANTIDADE_FILOSOFOS];
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    int i;
    pthread_mutex_init( &(mutex), NULL);
    for( i = 0 ; i< QUANTIDADE_FILOSOFOS; i++ )
    {
        pthread_mutex_init( &(mutex_filosofos[i]), NULL );
    }

    for(i=0;i<QUANTIDADE_FILOSOFOS;i++){

        pthread_create(&jantar[i], &attr,filosofo, NULL);
    }
    for(i=0;i<QUANTIDADE_FILOSOFOS;i++){
        pthread_join(jantar[i],NULL);
    }
    pthread_exit(NULL);
    return 0;
}

