// File: server.c
// Authors: TJ Maynes and Chris Migut
#include <pthread.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <semaphore.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define _REENTRANT
#define SHMKEY ((key_t) 5555)
#define BUFFER_SIZE 13
#define PORT 8080
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

}

int
main(){
  int socketSetup, clientSocket, newSocket, clientLength;
  struct sockaddr_in client, server;

  // create tcp socket
  socketSetup = socket(AF_INET, SOCK_STREAM, 0);

  // error check socket creation
  if (socketSetup == -1){
    perror("Could not create socket!");
    return -1;
  }
  // set up server
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons(PORT);

  // bind socket
  if(bind(socketSetup,(struct sockaddr *)&server, sizeof(server)) < 0){
    perror("bind failed");
    return -1;
  }

  // listen for a specific number of connections on the socket
  listen(socketSetup, CLIENTS);

  printf("Waiting for incoming connections...");

  clientLength = sizeof(struct sockaddr_in);
  
  /* Create threads for each client connection */
  // may not need &attr[0] => NULL (default values)
  while((clientSocket = accept(socketDescription, (struct sockaddr*)&client, (socklen_t*)&clientLength))){

    pthread_t tid;              /* process id for thread 1 */
    pthread_attr_t attr[1];     /* attribute pointer array */
    
    if(pthread_create(&tid, NULL, &handler, (void *)i) < 0){
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
