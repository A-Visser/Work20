#include "pipe_networking.h"
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

/*=========================
  server_handshake
  args: int * to_client
  Performs the server side pipe 3 way handshake.
  Sets *to_client to the file descriptor to the downstream pipe.
  returns the file descriptor for the upstream pipe.
  =========================*/
int server_handshake(int *to_client) {
    mkfifo(WKP, 0644);
    int from_client = open(WKP, O_RDONLY);
    char handshake[HANDSHAKE_BUFFER_SIZE];
    read(from_client, handshake, HANDSHAKE_BUFFER_SIZE);
    printf("Received handshake: %s\n", handshake);
    remove(WKP);
    *to_client = open(handshake, O_WRONLY);
    write(*to_client, ACK, sizeof(ACK));
    char response[100];
    read(from_client, response, sizeof(response));
    remove(handshake);
    return from_client;
}


/*=========================
  client_handshake
  args: int * to_server
  Performs the client side pipe 3 way handshake.
  Sets *to_server to the file descriptor for the upstream pipe.
  returns the file descriptor for the downstream pipe.
  =========================*/
int client_handshake(int *to_server) {
    int pid = getpid();
    char handshake[100];
    sprintf(handshake, "%d", pid);
    mkfifo(handshake, 0644);

    *to_server = open(WKP, O_WRONLY);
    write(*to_server, handshake, sizeof(handshake));

    int from_server = open(handshake, O_RDONLY);
    read(from_server, handshake, sizeof(handshake));
    if (strcmp(handshake, ACK) == 0) {
        printf("Received acknowledgement from server!\n");
        remove(handshake);
        char * response = "Success";
        printf("%s\n", response);
        write(*to_server, response, sizeof(response));
    }

    return from_server;
}
