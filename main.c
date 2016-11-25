#include "etcp.h"
#define DEFAULT_PORT "5555"
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Mswsock.lib")
char * program_name = "Multithread server";
int ishistory = 0;
void clientSend(SOCKET s, char *sendstr);

int main(int argc, char** argv) {

    char *hname, *sname; //получаемые аргументы (адрес хоста и номер сервиса(порта)
	SOCKET s;
    char sendbuf[MAX_STR_LEN];
    char receivebuf[MAX_STR_LEN];
    bzero(sendbuf, sizeof(sendbuf));
    bzero(receivebuf, sizeof(receivebuf));
    printf("PROGRAM: %s\n", program_name);
	sname = "7500";
	hname = "192.168.1.143";

	//hname = "88.201.178.18";
	INIT();
	// Шаг 1 - инициализация библиотеки Winsock
	WSADATA wsaData;
	int iResult = WSAStartup(0x0202, &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed: %d\n", iResult);
		return 1;
	}

    s = tcp_client(hname, sname);
	char tmpsendbuf[50];
	char *money;

	//int rc;
	//"консоль"
	while (1) {
		printf("-------------\nList  of  commands:\n1: new <user> <password> -- Create new account\n2: aut <user> <password> -- Member Login"
				"\n3: getinfo -- Account information \n4: gettr -- Transactions history \n5: send <user> <money> -- Send money to the user"
				"\n6: put <money> -- Add funds\n7: get <money> -- Withdraw of funds\n8: exit -- Sign Out\n-------------\n");
		printf("--------------------------\ninput command: ");
		fgets(tmpsendbuf, sizeof (tmpsendbuf), stdin);
		strcpy(sendbuf, tmpsendbuf);
		//проверка введенной команды и её синтаксиса
		if (strncmp(tmpsendbuf, "exit\n", 5) == 0) {
			printf("client is ending\n");
			break;
		}
		else if (strncmp(tmpsendbuf, "new ", 4) == 0) //отображаем список клиентов
		{
			strtok(tmpsendbuf, " ");//пропускаем aut
			if (strtok(NULL, " ") == NULL || strtok(NULL, " ") == NULL || strtok(NULL, " ") != NULL)
			{
				printf("incorrect syntax of command new\n");
				continue;
			}
			clientSend(s, sendbuf);
			continue;
		}
		else if (strncmp(tmpsendbuf, "aut ", 4) == 0) //отображаем список клиентов
		{
			strtok(tmpsendbuf, " ");//пропускаем aut
			if (strtok(NULL, " ") == NULL || strtok(NULL, " ") == NULL || strtok(NULL, " ") != NULL)
			{
				printf("incorrect syntax of command aut\n");
				continue;
			}
			clientSend(s, sendbuf);
			continue;
		}
		else if (strncmp(tmpsendbuf, "getinfo ", 8) == 0 || strncmp(tmpsendbuf, "getinfo\n",8) == 0)
		{
			strtok(tmpsendbuf, " "); // пропуск get
			if (strtok(NULL, " ") != NULL) // если что-то еще есть
			{
				printf("incorrect syntax of command get\nwithout spaces\n");
				continue;
			}
			clientSend(s, sendbuf);
			continue;
		}
		else if (strncmp(tmpsendbuf, "gettr ", 6) == 0 || strncmp(tmpsendbuf, "gettr\n", 6) == 0)
		{
			strtok(tmpsendbuf, " "); // пропуск get
			if (strtok(NULL, " ") != NULL) // если что-то еще есть
			{
				printf("incorrect syntax of command get\nwithout spaces\n");
				continue;
			}
			ishistory = 1;
			clientSend(s, sendbuf);
			continue;
		}
		else if (strncmp(tmpsendbuf, "send ", 5) == 0)
		{
			printf("%s\n", strtok(tmpsendbuf, " ")); // пропуск send
			printf("%s\n", strtok(NULL, " ")); // пропуск send
			money = strtok(NULL, " "); // пропуск пользователя
			if (money == NULL || atol(money) <= 0 || atol(money) > 1000000 || strtok(NULL, "") != NULL) // если что-то еще есть
			{
				printf("incorrect syntax of command put\n");
				printf("money= %d", money);
				continue;
			}
			clientSend(s, sendbuf);
			continue;
		}
		else if (strncmp(tmpsendbuf, "put", 3) == 0)
		{
			strtok(tmpsendbuf, " "); // пропуск get
			money = strtok(NULL, " "); //получили money
			if (money == NULL || atol(money) <= 0 || atol(money) > 1000000 || strtok(NULL, " ") != NULL) // если что-то еще есть
			{
				printf("incorrect syntax of command put\n");
				continue;
			}
			clientSend(s, sendbuf);
		}
		else if (strncmp(tmpsendbuf, "get", 3) == 0)
		{
			strtok(tmpsendbuf, " "); // пропуск get
			money = strtok(NULL, " "); //получили money
			if (money == NULL || atol(money) <= 0 || atol(money) > 1000000 || strtok(NULL, " ") != NULL) // если что-то еще есть
			{
				printf("incorrect syntax of command put\n");
				continue;
			}
			clientSend(s, sendbuf);
		}
		else
		{
			printf("incorrect Command\n");
			continue;
		}
		continue;
	}
	return (EXIT_SUCCESS);
	closesocket(s);
}

void clientSend(SOCKET s, char *sendstr) //посылка запроса серверу и получение ответа
{
	int rc;
	char recbuf[1000];
	printf("Response: \n");
	if (ishistory == 1)
	{
		sendLine(s, sendstr);
		rc = recvLine(s, recbuf, sizeof(recbuf));
		while (rc>1)
		{
			printf("%s\n", recbuf);
			rc = recvLine(s, recbuf, sizeof(recbuf));
			if (rc <= 0)

			{
				printf("Connection refused\nwait 5 sec\n");
				exit(100);
			}
		}
		ishistory = 0;
		return;
		if (rc <= 0)
		{
			printf("Connection refused\nwait 5 sec\n");
			sleep(5);
			exit(100);
		}
	}
	else
	{
		sendLine(s, sendstr);
		rc = recvLine(s, recbuf, sizeof(recbuf));
		if (rc <= 0)
		{
			printf("Connection refused\nwait 5 sec\n");
			sleep(5);
			exit(100);
		}
		printf("%s\n", recbuf);
	}
}
