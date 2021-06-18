#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <ctype.h>
// #include <pthread.h>

#define PORT 8080
#define HANDSHAKE "800"
#define SUCCESS "200"

char cred[50];
int isValid = 0;

void clear_buffer(char *b)
{
  int i;
  for (i = 0; i < BUFSIZ; i++)
    b[i] = '\0';
}

int cekCred(int sock)
{
  FILE *fp = fopen("user.txt", "r");
  char *locCred = malloc(sizeof(char) * 50);
  int q;
  for (q = 0; q < 50; q++)
    locCred[q] = '\0';

  char buffer[BUFSIZ];
  char sent[BUFSIZ];
  int isValid = 0;

  memset(buffer, 0, sizeof(buffer));
  read(sock, buffer, BUFSIZ);

  while (fscanf(fp, "%s", locCred) != EOF)
  {
    if (!strcmp(locCred, buffer))
    {
      isValid = 1;
      strcpy(cred, locCred);
      break;
    }
    else
    {
      for (q = 0; q < 100; q++)
        locCred[q] = '\0';
    }
  }

  sprintf(sent, "%d", isValid);
  send(sock, sent, sizeof(sent), 0);

  return isValid;
}

int komper(char const *a, char const *b)
{
  for (;; a++, b++)
  {
    int d = tolower((unsigned char)*a) - tolower((unsigned char)*b);
    if (d != 0 || !*a)
      return d;
  }
}

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

void notCmd(int sock)
{
  printf("%s\n", "INVALIDDDD");
  send(sock, "Command Invalid", 100, 0);
}

int isCreateUser(int sock, char com[][100])
{
  if (komper(com[0], "create") != 0)
  {
    notCmd(sock);
    return 0;
  }
  if (komper(com[1], "user") != 0)
  {
    notCmd(sock);
    return 0;
  }
  if (komper(com[3], "identified") != 0)
  {
    notCmd(sock);
    return 0;
  }
  if (komper(com[4], "by") != 0)
  {
    notCmd(sock);
    return 0;
  }
  send(sock, "CMD : [Create User]", 100, 0);
  return 1;
}

void mainApp(int sock)
{
  char buffer[BUFSIZ];
  char cmd[BUFSIZ] = {0};
  char cred[100];
  char com[20][100];
  int isRoot = 0;
  int q;
  for (q = 0; q < 100; q++)
    cred[q] = '\0';

  printf("ROOT : %s\n", cred);

  // receive root status
  read(sock, buffer, 1);
  if (buffer[0] == '1')
  {
    isRoot = 1;
  }

  printf("%d\n", isRoot);

  while (1)
  {
    clear_buffer(buffer);

    if (isValid == 0)
    {
      if (cekCred(sock) == 1)
        isValid = 1;
    }
    else if (isValid == 1)
    {
      read(sock, cmd, BUFSIZ);
      printf("Command : %s\n", cmd);

      int i = splitString(com, cmd, " ,=();");

      if (i == 6) // untuk command yang panjangnya 5 kata
      {
        if (isCreateUser(sock, com))
        {
          printf("crete user : [%s:%s]\n", com[2], com[5]);
          // do create user
        }
      }
      else if (i == 4) // untuk command yang panjangnya 4 kata
      {
      }
      else if (i == 6) // untuk command yang panjangnya 6 kata
      {
      }
      else
      {
        notCmd(sock);
      }
    }
  }
  printf("BREAKED !\n");
}

void handle_new_connection(int socketfd)
{
  char buffer[BUFSIZ];
  int valread;

  clear_buffer(buffer);
  valread = read(socketfd, buffer, BUFSIZ);
  fprintf(stdout, "Receive connection handshake %s\n", buffer);

  send(socketfd, SUCCESS, strlen(SUCCESS), 0);
  fprintf(stdout, "Sent response %s\n", SUCCESS);

  mainApp(socketfd);
}

void launchServer()
{
  int server_fd, socketfd, valread;
  struct sockaddr_in address;
  int opt = 1;
  int addrlen = sizeof(address);
  char buffer[BUFSIZ] = {0};

  pthread_t tid[100];
  int connections = 0;

  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
  {
    perror("socket failed");
    exit(EXIT_FAILURE);
  }

  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
  {
    perror("setsockopt");
    exit(EXIT_FAILURE);
  }

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(PORT);

  if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
  {
    perror("bind failed");
    exit(EXIT_FAILURE);
  }

  if (listen(server_fd, 3) < 0)
  {
    perror("listen failed");
    exit(EXIT_FAILURE);
  }

  while (1)
  {
    printf("While launchServer()\n");
    if ((socketfd = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
    {
      perror("accept failed");
      exit(EXIT_FAILURE);
    }
    handle_new_connection(socketfd);
    // pthread_create(&(tid[connections]), NULL, &handle_new_connection, &socketfd);
    // pthread_join(tid[connections], NULL);
    // connections++;
  }
}

int main(int argc, char *argv[])
{
  launchServer();

  return 0;
}