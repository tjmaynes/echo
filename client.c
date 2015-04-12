// File: client.c
// Authors: TJ Maynes and Chris Migut
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>

#define PORT 8080
#define IPADDR "127.0.0.1" // change to desired remote ip address to server

int
main(){
  int socket_setup;
  struct sockaddr_in server;

  puts("Welcome to the Client Connection Program!");

  // create tcp connection
  socket_setup = socket(AF_INET, SOCK_STREAM, 0);

  // error check the socketSetup
  if (socket_setup == -1){
    perror("Failed to create socket!");
    return -1;
  }

  // setup server address and port, AF_INET => internet protocol
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = inet_addr(IPADDR);
  server.sin_port = htons(PORT);

  // connect to remote server
  if (connect(socket_setup, (struct sockaddr *)&server, sizeof(server)) < 0){
    perror("Failed to connect to remote server");
    return -1;
  }

  // inform user that they are connected to the remote server (include info)
  printf("Connected to server: %s on Port: %d\n", IPADDR, PORT);

  // declare messages
  char message_received[50];
  char message_response[10];

  // receive initial message from remote server
  if (recv(socket_setup, message_received, sizeof(message_received), 0) < 0){
    perror("Failed to receive message!");
    return -1;
  }

  // print out and submit message
  printf("\n%s", message_received);
  scanf ("%[^\n]%*c", message_response);

  // send message to remote server
  if (send(socket_setup, message_response, sizeof(message_response), 0) < 0){
    perror("Failed to send message!");
    return -1;
  }

  // receive final message from server
  if (recv(socket_setup, message_received, sizeof(message_received), 0) < 0){
    perror("Failed to receive message!");
    return -1;
  }
  printf("\n%s\n", message_received);

  // destroy socket
  close(socket_setup);

  // thank the user for trying out the program
  printf("\nThank you for using this program!\n");
  return 0;
}
