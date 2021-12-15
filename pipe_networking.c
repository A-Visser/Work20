#include "pipe_networking.h"


/*=========================
  server_handshake
  args: int * to_client
  Performs the client side pipe 3 way handshake.
  Sets *to_client to the file descriptor to the downstream pipe.
  returns the file descriptor for the upstream pipe.
  =========================*/
int server_handshake(int *to_client) {
    printf("Welcome to Server Client System\n");
    printf("Use \"exit\" to end proccess\n");
    int sToCerr = mkfifo(WKP, 0644);
    if (sToCerr == -1){
        printf("Error: %s\n", strerror(errno));
        return 0;
    }

    int from_client = open(WKP, O_RDONLY);
    if (from_client == -1){
        printf("Error: %s\n", strerror(errno));
        return 0;
    }


    char message[HANDSHAKE_BUFFER_SIZE];
    int foo = read(from_client, message, HANDSHAKE_BUFFER_SIZE);
    if (foo == -1){
        printf("Error: %s\n", strerror(errno));
        return 0;
    }


    *to_client = open(message, O_WRONLY);
    if (*to_client == -1){
        printf("Error: %s\n", strerror(errno));
        return 0;
    }

    int goo = write(*to_client, ACK, sizeof(ACK));
    close(*to_client);
    char pipename[HANDSHAKE_BUFFER_SIZE];
    strncpy(pipename, message, sizeof(message));
    if (goo == -1){
        printf("Error: %s\n", strerror(errno));
        return 0;
    }

    int hoo = read(from_client, message, sizeof(message));
    if (hoo == -1){
        printf("Error: %s\n", strerror(errno));
        return 0;
    }
    if (strcmp(message, ACK) == 0){
        printf("Three ways handshake achieved!\n");
    }
    else{
        printf("Three way handshake failed!\n");
    }
    while(1){
      char line[STRINGSIZE];
      from_client = open(WKP, O_RDONLY);
      read(from_client, line, STRINGSIZE);
      if(!strncmp(line, "exit", 4)){
          remove(WKP);
          exit(0);
        }
      close(from_client);
      int i;
      for(i = 0; i < strlen(line); i++){
        line[i] = toupper(line[i]);
      }
      *to_client = open(pipename, O_WRONLY);
      write(*to_client, line, STRINGSIZE);
      close(*to_client);
    }

    remove(pipename);
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

    char pidp[1024];
    sprintf(pidp, "%d", getpid());

    mkfifo(pidp, 0644);
    *to_server = open(WKP, O_WRONLY);
    if (*to_server == -1){
        printf("Error: %s\n", strerror(errno));
        return 0;
    }
    int foo = write(*to_server, pidp, HANDSHAKE_BUFFER_SIZE);
    if (foo == -1){
        printf("Error: %s\n", strerror(errno));
        return 0;
    }
    int from_server = open(pidp, O_RDONLY);
    if (from_server == -1){
        printf("Error: %s\n", strerror(errno));
        return 0;
    }
    char message[HANDSHAKE_BUFFER_SIZE];
    int goo = read(from_server, message, HANDSHAKE_BUFFER_SIZE);
    if (goo == -1){
        printf("Error: %s\n", strerror(errno));
        return 0;
    }
    int hoo = write(*to_server, ACK, sizeof(ACK));
    if (hoo == -1){
        printf("Error: %s\n", strerror(errno));
        return 0;
    }
    close(*to_server);
    printf("Three way handshake achieved!\n");
    while(1){
      char line[STRINGSIZE];
      printf("input string: \n");
      fgets(line, STRINGSIZE, stdin);
      *to_server = open(WKP, O_WRONLY);
      int hoo = write(*to_server, line, STRINGSIZE);

      if (hoo == -1){
          printf("Error: %s\n", strerror(errno));
          return 0;
      }
      close(*to_server);
      if(!strncmp(line, "exit", 4)){
        exit(0);
      }
      open(pidp, O_RDONLY);
      goo = read(from_server, line, STRINGSIZE);
      close(from_server);
      if (goo == -1){
          printf("Error: %s\n", strerror(errno));
          return 0;
      }
      printf("Processed String: %s \n", line);
    }
    return from_server;
}
