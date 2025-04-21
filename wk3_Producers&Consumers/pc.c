#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include "eventbuf.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
sem_t *sem;

int *event_buffer = 0;
int producer = 0;
int consumer = 0;
int event;
int free_event;


sem_t *sem_open_temp(const char *name, int value) {
    sem_t *sem;

    // Create the semaphore
    if ((sem = sem_open(name, O_CREAT, 0600, value)) == SEM_FAILED)
        return SEM_FAILED;

    // Unlink it so it will go away after this process exits
    if (sem_unlink(name) == -1) {
        sem_close(sem);
        return SEM_FAILED;
    }

    return sem;
}   

void* producer_thread(void *arg){
    int *id = arg;
    //create event buff
    struct eventbuf *eb = eventbuf_create();
        
    //wait on spaces semaphore that counts number of free spaces,
    sem = sem_open_temp("spaces", 1);

    //each producer will add the number of events from command line
    int num_events = ((producer * 100) + event);

    //when producer wakes up, means room for more, 
    //locks mutex around queue, add event, print production message, unlock queue mutex
    sem_wait(sem);

    eventbuf_add(eb, num_events);
    
    printf("P%d: %s, %d\n", *id, "adding event", num_events);
    //post to items semaphore indicating an event is available for consumption
    sem_post(sem);
    return NULL;
}

void *consumer_thread(void * arg){
    int *id = arg;
    //create event buff
    struct eventbuf *eb = eventbuf_create();
    //items, semaphore that counts number of items in queue
    sem = sem_open_temp("items", 0);

    //each producer will add the number of events from command line
    int num_events = ((producer * 100) + event);

    //consumer waits for item semaphore
    sem_wait(sem);
    //get event and print consumption message, unlock the queue mutex
    eventbuf_get(eb);
    printf("C%d: %s, %d\n", *id, "got event", num_events);
    
    //post to spaces semaphore indicates are available
    eventbuf_empty(eb);
    sem_post(sem);
    return NULL;
}

int main (int argc, char* argv[]){

    //sem to notify consumers that they should exit
    //sem = sem_open_temp("PC_Sem", consumer);

    //parse command line
    if(argc < 4){
        printf("%s", "error incorrect number of arguments entered\n");
        exit(1);
    }
    producer = atoi(argv[1]);
    consumer = atoi(argv[2]);
    event = atoi(argv[3]);
    free_event = atoi(argv[4]);

    event_buffer = calloc(event, sizeof *event_buffer);

    pthread_t *pthread = calloc(producer, sizeof *pthread);
    int *procID_thread = calloc(producer, sizeof *procID_thread);

    pthread_t *cthread = calloc(consumer, sizeof *cthread);
    int *CID_thread = calloc(consumer, sizeof *CID_thread);

    //start the correct number of producer threads
    for(int i = 0; i < producer; i++){
        //create a unique id for each thread
        procID_thread[i] = i;
        pthread_create(&pthread[i], NULL, producer_thread, procID_thread +i);

    }
    
    //start the correct number of consumer threads
    for(int j = 0; j < consumer; j++){
        //create a unique id for each thread
        CID_thread[j] = j;
        pthread_create(&cthread[j], NULL, consumer_thread, CID_thread + j);

    }
    
    //wait for all produce threads to complete
    for(int k = 0; k < producer; k++){
        pthread_join(pthread[k], NULL);
    }

    //notify all the consumer threads that they're done
    for(int x = 0; x < consumer; x++){
        sem_close(sem);
    }

    //wait for all consumer threads to complete
    for(int z = 0; z < consumer; z++){
        pthread_join(cthread[z], NULL);
    }

    //free the event buffer
    free(event_buffer);
    free(pthread);
    free(procID_thread);
    free(cthread);
    free(CID_thread);
}
