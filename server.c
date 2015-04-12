// File: server.c
// Authors: TJ Maynes and Chris Migut
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
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
void* handler(void* args); // get handler

typedef struct {
  int connection_count;
  char *messages[BUFFER_SIZE];
} shared_mem;

// shared memory buffer of values and a counter
shared_mem buffer;

// declare semaphores
sem_t sem1; // empty
sem_t sem2; // mutex
sem_t sem3; // full

int
main(){
  int socket_setup, client_socket, client_length = 0;
  struct sockaddr_in client, server;

  // initialize semaphores
  sem_init(&sem1, 0, BUFFER_SIZE); // initialize empty
  sem_init(&sem2, 0, 1);           // initialize mutex
  sem_init(&sem3, 0, 0);           // initialize full

  // create tcp socket
  socket_setup = socket(AF_INET, SOCK_STREAM, 0);

  puts("Welcome to the Server!");

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
  listen(socket_setup, 2);

  puts("Waiting for incoming connections...");

  client_length = sizeof(struct sockaddr_in);

  /* Create threads for each client connection */
  // may not need &attr[0] => NULL (default values)
  pthread_t threads = NULL;
  while((client_socket = accept(socket_setup, (struct sockaddr*)&client, (socklen_t*)&client_length)) > 0){
    // debug => thread id
    //printf("\n Created new thread (%u) ... \n", threads);

    // connection made
    puts("\n....Accepted Connection....");

    /* produce an item in next produced */
    sem_wait(&sem1); // empty
    sem_wait(&sem2); // mutex
    buffer.connection_count++; // increment count
    /* add next produced to the buffer */
    sem_post(&sem2); // mutex
    sem_post(&sem3); // full

    if(pthread_create(&threads, NULL, &handler, (void *)&client_socket) < 0){
      perror("could not create pthread");
      return -1;
    }
  }
  pthread_join(threads, NULL);

  printf("----------------------------------------------------------------------\n");
  printf("Number of Client Connections  =  %d\n", buffer.connection_count);
  printf("\nMessages =  %s", &buffer.messages[BUFFER_SIZE]);
  printf("\nEnd of simulation\n\n");

  // destroy semaphores
  sem_destroy(&sem1); // destroy empty
  sem_destroy(&sem2); // destroy mutex
  sem_destroy(&sem3); // destroy full

  return 0;
}

// handler function
void* handler(void* args) {
  int socket = *(int*)args;
  int size_of_packet = 0;
  char *message_to_client;
  char message_from_client[10];

  message_to_client = "Enter a message to send (up to 10 characters): ";
  send(socket, message_to_client, strlen(message_to_client),0);

  while((size_of_packet = recv(socket, message_from_client, 10, 0)) > 0){
    message_from_client[size_of_packet] = '\0';

    puts(message_from_client);

    /* produce an item in next produced */
    sem_wait(&sem1); // empty
    sem_wait(&sem2); // mutex
    buffer.messages[CLIENTS % BUFFER_SIZE] = message_from_client; // circular buffer
    /* add next produced to the buffer */
    sem_post(&sem2); // mutex
    sem_post(&sem3); // full

    send(socket, message_from_client, 10, 0);
    memset(message_from_client, 0, 10);
  }

  if(size_of_packet == 0){
    puts("\n....Client disconnected....");
    fflush(stdout);
  }
  else if(size_of_packet == -1){
    perror("\nrecv failed");
  }

  // exit pthread
  pthread_exit(0);
}
