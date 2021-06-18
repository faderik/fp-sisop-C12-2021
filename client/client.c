#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/sendfile.h>

#define PORT 8080
#define HANDSHAKE "800"
#define SUCCESS "200"

char cred[50];
int isValid = 0;

int splitString(char res[][100], char toSplit[], const char delimiter[])
{
  int i = 0;
  char *tok = strtok(toSplit, delimiter);

  while (tok != NULL)
  {
    strcpy(res[i++], tok);
    tok = strtok(NULL, delimiter);
  }

  return i;
}

void clear_buffer(char *b)
{
  int i;
  for (i = 0; i < BUFSIZ; i++)
    b[i] = '\0';
}

int isRooot()
{
  if (geteuid() == 0)
    return 1;
  else
    return 0;
}

int create_socket()
{
  struct sockaddr_in address;
  int socketfd, valread;
  struct sockaddr_in serv_addr;
  char buffer[BUFSIZ] = {0};
  if ((socketfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
    fprintf(stdout, "\n Socket creation error \n");
    return -1;
  }

  memset(&serv_addr, '0', sizeof(serv_addr));

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(PORT);

  if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
  {
    fprintf(stdout, "\nInvalid address/ Address not supported \n");
    return -1;
  }

  if (connect(socketfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
  {
    fprintf(stdout, "\nConnection Failed \n");
    return -1;
  }

  return socketfd;
}

int cekCred(int sock, char *creden)
{
  char buffer[BUFSIZ];
  char cred[100];
  strcpy(cred, creden);

  send(sock, cred, sizeof(cred), 0);
  read(sock, buffer, sizeof(buffer));

  if (buffer[0] == 1 + '0')
  {
    printf("Auth Success\n");
    return 1;
  }
  else
  {
    printf("Auth Failed\n");
    exit(0);
    return 0;
  }
}

void menuApp(int sock)
{
  char buffer[BUFSIZ];
  int valread;
  int isLoggedIn = 0;
  char cmd[BUFSIZ];
  char pkt[BUFSIZ];
  char pub[100];
  char thn[4];
  char filePth[100];

  // sending root status
  if (isRooot())
    send(sock, "1", 1, 0);
  else
    send(sock, "0", 1, 0);

  while (1)
  {
    clear_buffer(buffer); // untuk read
    clear_buffer(pkt);    // untuk send

    // for (int q = 0; q < 100; q++)
    //   memset(com[q], 0, 100);
    // strcpy(com[q], "\0");

    if (isValid == 0)
    {
      printf("Check Cred ....\n");
      if (cekCred(sock, cred))
        isValid = 1;
    }
    else if (isValid == 1)
    {
      printf("Enter command : \n");
      scanf(" %[^\n]c", cmd);

      send(sock, cmd, BUFSIZ, 0); // sending cmd to db
      read(sock, buffer, BUFSIZ); // receive respond from db

      printf("%s\n", buffer);
    }
  }
  printf("BREAKED !\n");
}

void connect_to_server(int socketfd)
{
  char buffer[BUFSIZ];
  int valread;

  send(socketfd, HANDSHAKE, strlen(HANDSHAKE), 0);

  fprintf(stdout, "Menunggu koneksi dari server...\n");

  clear_buffer(buffer);
  valread = read(socketfd, buffer, BUFSIZ);
  fprintf(stdout, "Terhubung dengan server\n");

  menuApp(socketfd);
}

int main(int argc, char *argv[])
{
  char uname[20];
  char pwd[20];

  if (isRooot())
  {
    sprintf(cred, "%s:%s", "root", "root");
    isValid = 1;
    printf("%s\n", cred);
  }
  else
  {
    if (argc > 1 && strcmp(argv[1], "-u") == 0)
      strcpy(uname, argv[2]);
    else
    {
      printf("Authentication Failed\n");
      exit(0);
    }
    if (argc > 3 && strcmp(argv[3], "-p") == 0)
      strcpy(pwd, argv[4]);
    else
    {
      printf("Authentication Failed\n");
      exit(0);
    }

    sprintf(cred, "%s:%s", uname, pwd);
    printf("%s\n", cred);
  }

  int socketfd = create_socket();
  if (socketfd == -1)
    exit(0);

  connect_to_server(socketfd);

  return 0;
}