#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <signal.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/stat.h>

#define PORT 20000
#define LENGTH 512
#define SERVER_ADDR "127.0.0.1"

void error(const char *msg)
{
  perror(msg);
  exit(1);
}

int main(int argc, char *argv[]){
  /* Variable Definition */
  int sockfd;
  int nsockfd;
  char revbuf[LENGTH];
  struct sockaddr_in remote_addr;

  /* Get the Socket file descriptor */
  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
    fprintf(stderr, "ERROR: Failed to obtain Socket Descriptor! (errno = %d)\n",errno);
    exit(1);
  }

  /* Fill the socket address struct */
  remote_addr.sin_family = AF_INET;
  remote_addr.sin_port = htons(PORT);
  inet_pton(AF_INET, SERVER_ADDR, &remote_addr.sin_addr);
  bzero(&(remote_addr.sin_zero), 8);

  /* Try to connect the remote */
  if (connect(sockfd, (struct sockaddr *)&remote_addr, sizeof(struct sockaddr)) == -1){
    fprintf(stderr, "ERROR: Failed to connect to the host! (errno = %d)\n",errno);
    exit(1);
  }
  else
    printf("[Client] Connected to server at port %d...ok!\n", PORT);

  /*If not specify file name, it will input.txt*/
  char* fs_name;
  if(argc == 1){
    fs_name = "input.txt";
  }
  else if(argc == 2){
    fs_name = argv[1];
  }
  else{
    printf("Wrong argument input\n");
    printf( "Usage: %s filename", argv[0] );
    exit(1);
  }

  /* If file is empty will close connect */
  struct stat stat_record;
  if(stat(fs_name, &stat_record)) printf("%s", strerror(errno));
  else if(stat_record.st_size <= 1){  //file is empty
    printf("File is empty.\n");
    close (sockfd);
    printf("[Client] Connection lost.\n");
    return (0);
  }

  /* Send File to Server */
  char sdbuf[LENGTH];
  printf("[Client] Sending %s to the Server... ", fs_name);
  FILE *fs = fopen(fs_name, "r");
  if(fs == NULL){
    printf("ERROR: File %s not found.\n", fs_name);
    exit(1);
  }

  bzero(sdbuf, LENGTH);
  int fs_block_sz;
  while((fs_block_sz = fread(sdbuf, sizeof(char), LENGTH, fs)) > 0){
    if(send(sockfd, sdbuf, fs_block_sz, 0) < 0){
      fprintf(stderr, "ERROR: Failed to send file %s. (errno = %d)\n", fs_name, errno);
      break;
    }
    bzero(sdbuf, LENGTH);
  }
  printf("Ok File %s from Client was Sent!\n", fs_name);

  /* Receive File from Server */
  printf("[Client] Receiveing file from Server and saving it as final.txt...");
  char* fr_name = "final.txt";
  FILE *fr = fopen(fr_name, "w");
  if(fr == NULL)
    printf("File %s Cannot be opened.\n", fr_name);
  else{
    bzero(revbuf, LENGTH);
    int fr_block_sz = 0;
    while((fr_block_sz = recv(sockfd, revbuf, LENGTH, 0)) > 0){
      int write_sz = fwrite(revbuf, sizeof(char), fr_block_sz, fr);
      if(write_sz < fr_block_sz){
        error("File write failed.\n");
      }
      bzero(revbuf, LENGTH);
      if (fr_block_sz == 0 || fr_block_sz != 512){
        break;
      }
    }
    if(fr_block_sz < 0){
      if (errno == EAGAIN){
        printf("recv() timed out.\n");
      }
      else{
        fprintf(stderr, "recv() failed due to errno = %d\n", errno);
      }
    }
      printf("Ok received from server!\n");
      fclose(fr);
  }
  close (sockfd);
  printf("[Client] Connection lost.\n");
  return (0);
}
