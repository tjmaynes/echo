// File: server.c
// Authors: TJ Maynes and Chris Migut
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define _REENTRANT
#define SHMKEY ((key_t) 5555)
#define CLIENTS 2
#define BUFFER_SIZE 10 * CLIENTS
#define PORT 8080

// function headers
void* handler(void* args); // get

typedef struct {
  int connection_count;
  char messages[BUFFER_SIZE];
} shared_mem;

// shared memory buffer of values and a counter
shared_mem buffer;

// declare semaphores
sem_t sem1; // empty
sem_t sem2; // mutex
sem_t sem3; // full

// handler function
void* handler(void* args) {
  int socket = *(int*)args;
  int size_of_packet = 0;
  char message_from_client[BUFFER_SIZE/CLIENTS];

  while((size_of_packet = recv(socket, message_from_client, BUFFER_SIZE / CLIENTS, 0)) > 0){

    // indicate end of message
    message_from_client[size_of_packet] = '\0';

    //

    // produce an item in next produced
    sem_wait(&sem1); // empty
    sem_wait(&sem2); // mutex
    //buffer.messages[BUFFER_SIZE/CLIENTS] = message_from_client; // circular buffer
    // add next produced to the buffer
    sem_post(&sem2); // mutex
    sem_post(&sem3); // full

  }
  // exit pthread
  pthread_exit(0);
}

int
main(){
  int socket_setup, client_socket, client_length, shmid = 0;
  struct sockaddr_in client, server;
  pthread_t threads;

  // create tcp socket
  socket_setup = socket(AF_INET, SOCK_STREAM, 0);

  // error check socket creation
  if (socket_setup == -1){
    perror("Could not create socket!");
    return -1;
  }

  // set up server
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons(PORT);

  // bind socket
  if(bind(socket_setup,(struct sockaddr *)&server, sizeof(server)) < 0){
    perror("bind failed");
    return -1;
  }

  // listen for a specific number of connections on the socket
  listen(socket_setup, CLIENTS);

  printf("Waiting for incoming connections...");

  client_length = sizeof(struct sockaddr_in);

  /* Create threads for each client connection */
  // may not need &attr[0] => NULL (default values)
  while((client_socket = accept(socket_setup, (struct sockaddr*)&client, (socklen_t*)&client_length))){

    // connection made
    printf("Accepted Connection....");

    if(pthread_create(&threads, NULL, &handler, (void *)&client_socket) < 0){
      perror("could not create pthread");
      return -1;
    }
    sleep(2);
  }

  printf("Number of Client Connections  =  %d\n", buffer.connection_count);
  printf("\nMessages: %s", buffer.messages);
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

  return 0;
}
