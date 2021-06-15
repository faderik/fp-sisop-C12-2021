#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int isRoot()
{
  // return geteuid() == 0;
}

char *setCred(char uname[], char pwd[])
{
  char *cred = malloc(sizeof(char) * 41);
  sprintf(cred, "%s:%s", uname, pwd);

  return cred;
}

int main(int argc, char *argv[])
{
  char uname[20];
  char pwd[20];

  // printf("%d\n", argc);
  // printf("%s\n", argv[1]); // -u
  // printf("%s\n", argv[2]); // username
  // printf("%s\n", argv[3]); // -p
  // printf("%s\n", argv[4]); // password

  // strcpy(uname, argv[2]);
  // strcpy(pwd, argv[4]);

  printf("%s\n", setCred(argv[2], argv[4]));

  return 0;
}