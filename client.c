// File: client.c
// Authors: TJ Maynes and Chris Migut
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>

#define PORT 8080
#define IPADDR "127.0.0.1"

int
main(){
  int socket_setup;
  struct sockaddr_in server;

  printf("Welcome to the Client Connection Program!");

  // create tcp connection
  socket_setup = socket(AF_INET, SOCK_STREAM, 0);

  // error check the socketSetup
  if (socket_setup == -1){
    perror("Failed to create socket!");
    return -1;
  }
  
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = inet_addr(IPADDR);
  server.sin_port = htons(PORT);

  // connect to remote server
  if (connect(socket_setup, (struct sockaddr *)&server, sizeof(server)) < 0){
    perror("Failed to connect to remote server!");
    return -1;
  }

  printf("\nConnected to server: %s on Port: %d", IPADDR, PORT);

  while(true){
    // declare messages
    char* message_sent;
    char* message_response;
    int decision = 0;
    
    // create arrays
    message_sent = malloc(1000);
    message_received = malloc(2000);

    // input a message to send to server
    printf("\nEnter a message to send: ");
    scanf("%s", message_sent);

    // send message to remote server
    if (send(socket_setup, message_sent, sizeof(message_sent), 0) < 0){
      perror("Failed to send message!");
      return -1;
    }

    // receive message from remote server
    if (recv(socet_setup, message_received, sizeof(message_received), 0) < 0){
      perror("Failed to receive message!");
      return -1;
    } 
    printf("Message recieved from server: %s", message_received);

    // free message arrays
    free(message_sent);
    free(message_received);

    printf("\nWould you like to run this program again? (0 or 1)");
    scanf("%d", decision);
    if (decision == 0){
      break;
    }
  }
  printf("\nThank you for using this program!");
  return 0;
}
