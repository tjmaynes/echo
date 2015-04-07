// File: server.c
// Authors: TJ Maynes and Chris Migut
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <semaphore.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define _REENTRANT
#define SHMKEY ((key_t) 5555)
#define BUFFER_SIZE 13
#define PORT "8080"
#define CLIENTS 10

// function headers
void* handler(void* args); // get

typedef struct {
  int connection_count;
  char value[BUFFER_SIZE];
} shared_mem;

// shared memory buffer of values and a counter
shared_mem buffer;

// declare semaphores
sem_t sem1; // empty
sem_t sem2; // mutex
sem_t sem3; // full

// handler function
void* handler(void* args) {
  int handlerThread = (int)args;
  int client;

  // create tcp socket connection
  if((client = socket(AF_INET,SOCK_STREAM,0)) < 0 ){
    perror("Failed to create socket for client");
    exit(1);
  }

  fflush(stdout);
  pthread_exit(NULL);
}

int
main(){
  int i;                      /* count */
  int shmid;                  /* shared memory ID */
  pthread_t tid[CLIENTS];     /* process id for thread 1 */
  pthread_attr_t attr[1];     /* attribute pointer array */
  char *shmadd;
  shmadd = (char *) 0;

  /* Create and connect to a shared memory segment*/
  if ((shmid = shmget (SHMKEY, sizeof(int), IPC_CREAT | 0666)) < 0)
  {
    perror ("shmget");
    exit (1);
  }

  // initialize semaphores
  sem_init(&sem1, 0, BUFFER_SIZE); // initialize empty
  sem_init(&sem2, 0, 1);           // initialize mutex
  sem_init(&sem3, 0, 0);           // initialize full

  /* Required to schedule thread independently.*/
  pthread_attr_init(&attr[0]);
  pthread_attr_setscope(&attr[0], PTHREAD_SCOPE_SYSTEM);
  /* end to schedule thread independently */

  /* Create threads for each client connection */
  // may not need &attr[0] => NULL (default values)
  for (i = 0; i < CLIENTS; i++){
     if(pthread_create(&tid[i], &attr[0], &handler, (void *)i) < 0){
        perror("could not create pthread");
        exit(-1);
     }
     sleep(2);
  }

  /* Wait for the threads to finish */
  pthread_join(tid, NULL);

  printf("Number of Client Connections  =  %d\n", buffer.connection_count);
  printf("------------------------------------------------\n");
  printf("\t\t End of simulation\n");

  // destroy semaphores
  sem_destroy(&sem1); // destroy empty
  sem_destroy(&sem2); // destroy mutex
  sem_destroy(&sem3); // destroy full

  if ((shmctl (shmid, IPC_RMID, (struct shmid_ds *) 0)) == -1)
  {
    perror("shmctl");
    exit(-1);
  }
  exit(0);
}
