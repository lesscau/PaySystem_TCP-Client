#include <errno.h>
#include <stdlib.h>
#include <conio.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "skel.h"

#define MAX_STR_LEN 400 /*максимальная длина строк*/
#define MINBSDSOCKERR        ( WSAEWOULDBLOCK )
extern char *program_name; /* имя программы */

SOCKET tcp_client(char * hname, char * sname); //создание tcp-клиента
void set_address(char *, char *, struct sockaddr_in *, char *);
int inet_aton( char *cp, struct in_addr *pin );
int sendLine(SOCKET sock, char *str);  // посылка -//-
int sendn(SOCKET, char *, int, int flags);  //посылка n байт
int recvLine(SOCKET sock, char* buffer, int buffSize);
int recvn(SOCKET fd, char *bp, int len);
