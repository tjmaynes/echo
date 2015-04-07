// File: server.c
// Authors: TJ Maynes and Chris Migut
//
//

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
#define BUFFER_SIZE 15
#define PORT "8080"

// function headers
void* handler(void* args); // get

typedef struct
{
  int count;
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
  fflush(stdout);
}

int
main(){
  int     r = 0;
  int     i;
  int     shmid;  /* shared memoryry ID */
  pthread_t     tid1[1];     /* process id for thread 1 */
  pthread_attr_t     attr[1];     /* attribute pointer array */
  char               *shmadd;
  shmadd = (char *) 0;

  struct addrinfo hints, *res, *p;
  int status;

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

  /* Create the producer and consumer threads */
  pthread_create(&tid1[0], &attr[0], &handler, NULL);

  /* Wait for the threads to finish */
  pthread_join(tid1[0], NULL);

  printf("from parent counter  =  %d\n", buffer.count);
  printf("------------------------------------------------\n");
  printf("\t\t End of simulation\n");

  // destroy semaphores
  sem_destroy(&sem1); // destroy empty
  sem_destroy(&sem2); // destroy mutex
  sem_destroy(&sem3); // destroy full

  if ((shmctl (shmid, IPC_RMID, (struct shmid_ds *) 0)) == -1)
  {
    perror ("shmctl");
    exit (-1);
  }
  exit(0);
}
