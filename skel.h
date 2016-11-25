#ifndef SKEL_H
#define SKEL_H
//#ifdef __cplusplus
//extern "C"
//{
/* Версия Winsock. */
#include <winsock2.h>
#include <windows.h>


typedef unsigned int uint32_t;
#define EMSGSIZE    WSAEMSGSIZE
#define INIT() ( program_name = strrchr( argv[ 0 ], '/' ) ) ? program_name++ : ( program_name = argv[ 0 ] )
#define EXIT()      do { WSACleanup(); exit((s));} while (0)
#define CLOSE(s)    if (closesocket(s)) error(1,errno,"ошибка вызова close")
#define errno       (GetLastError())
#define set_errno(e)    SetLastError((e))
#define isvalidsock(s)  ((s)!=SOCKET_ERROR )
#define bzero(b, n)  memset ((b),0,(n));
#define sleep(t)    Sleep((t)*1000)
#define WINDOWS
//}
//#endif
#endif /* __SKEL_H__ */
