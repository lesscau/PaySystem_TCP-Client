//
// Created by lesscau on 23.11.2016.
//

#include "etcp.h"
#pragma comment(lib, "Ws2_32.lib")


SOCKET tcp_client(char *hname, char *sname) {
    int fl;
    SOCKET s;
    struct sockaddr_in peer;


    set_address(hname, sname, &peer, "tcp"); //по адресу и порту заполняем структуру

	s = socket(AF_INET, SOCK_STREAM, 0); //создаем сокет надежн с устан соединения
	if (s == INVALID_SOCKET)
	{
		perror("Error after socket1 (tcp_client)");
		exit(20);
	}
	printf("Client Get Socket: %d with hname: %s sname: %s\n", s, hname, sname);

	if (connect(s, (struct sockaddr *)&peer, sizeof (peer))) //сразу устанавливаем соединение
    {
        perror("Error after connect2 (tcp_client)");
        exit(21);
    }
    printf("Connected With %s:%s\n", hname, sname);

    return s;
}

void set_address(char *hname, char *sname, struct sockaddr_in *sap, char *protocol) {
    struct servent *sp;
    struct hostent *hp;
    char *endptr;
    short port;

    bzero(sap, sizeof ( *sap));                         //обнуляем структуру адреса
    sap->sin_family = AF_INET;
														//получаем адрес

    if (hname != NULL) {                                //если указано имя хоста (в виде строки, состоящей из чисел или символьной)
        if (!inet_aton(hname, &sap->sin_addr)) {        //пытаемся преобразовать строку в сетевой адрес ( с сетевым порядком байт)
														//считая строку = адресом в десятичной аннотации
            hp = gethostbyname(hname);                  //если не получилось, то пробуем получить адрес по имени хоста
            if (hp == NULL)
            {                                           //аргументы: статус (больше 0, то exit), номер ошибки, срока вывода и еще строки
				//error(1, 0, "Неизвестный хост: %s\n", hname); //если не получилось, то ощибка
				printf("Invalid address %s\n", hp);
            }

            sap->sin_addr = *(struct in_addr *) hp->h_addr;
        }
    }
    
	else
		sap->sin_addr.s_addr = htons(INADDR_ANY);       //ставим любой адресс (слушающий сокет)
														//получаем протокол и порт
	port = strtol(sname, &endptr, 0);                   //преобразуем номер порта
														//convert string to long integer (последний аргумент - основание СС, по умолчанию - десятичн)
	if (*endptr == '\0') {                              //endptr хранит первый непрвильный символ, если это конец строки, то все ок
		sap->sin_port = htons(port);                    //host to network short
	} else {                                            //не получилось преобразовать (встретились буквы)
		sp = getservbyname(sname, protocol);
		if (sp == NULL)
		{
			printf("Неизвестный сервис: %s\n", sname);
			exit(100);
		}
		sap->sin_port = sp->s_port;
	}

}


int inet_aton( char *cp, struct in_addr *pin )
{
	
    unsigned long rc;
   
    rc = inet_addr( cp );
    if ( rc == -1 && strcmp (cp, "255.255.255.255" ))
        return 0;
    pin->s_addr = rc;
    
    return 1;
	
	/*
	HOSTENT *hst;
	// преобразование IP адреса из символьного в сетевой формат
	if (inet_addr(cp) != INADDR_NONE)
		pin->s_addr = inet_addr(cp);
	else
	{
		// попытка получить IP адрес по доменному имени сервера
		if (hst = gethostbyname(cp))
			// hst->h_addr_list содержит не массив адресов,
			// а массив указателей на адреса
			((unsigned long *)&pin->s_addr)[0] =
			((unsigned long **)hst->h_addr_list)[0][0];
		else
		{
			printf("Invalid address %s\n", cp);
			WSACleanup();
			return 0;
		}
	}
	return 1;
	*/
}

int recvn(SOCKET fd, char *bp, int len) {

	return recv(fd, bp, len, 0);  //ждем пока не придет len байт (может так же вернуть
	//меньше байт или завершиться ошибкой
}

// Прием сообщений, разделенных разделителями
//возвращаем длину сообщения
int recvLine(SOCKET sock, char* buffer, int buffSize) {
	char* buff = buffer;        //для получения размера сообщения с нулевым символом
	char* currPosPointer;       //позиция, с которой пишутся байты в buffer
	int count = 0;              //число прочитанных байт
	char tempBuf[100];          //внутренний приемный буфер
	char currChar;              //текущий символ, записываемый в выходной буфер
	int tmpcount = 0;           //так же число прочитанных байт, необходимое для удаления символов из буфера, если не
                           //встретился разделитель
	while (--buffSize > 0) {                    /* по одному символу выводим во внешн буфер */
		if (--count <= 0) {                     /* пока не обработали все прочитанные символы, не зайдем в if*/
			recvn(sock, tempBuf, tmpcount);   /*Удаляем  обработанные  символы  из  потока (изначально ничего не удаляем)*/
			count = recv(sock, tempBuf, sizeof (tempBuf), 0); /* читаем без удаления из потока */
			if (count <= 0){                    /* проверка на ошибки */
				return count;
			}
			currPosPointer = tempBuf;           /*ставим указатель для анализа строки*/
			tmpcount = count;                   /*количество прочитанных байт*/
			//buff = buffer; /*и конец уже прочитанной строки*/
		}
		currChar = *currPosPointer++;           /*получаем символ из анализируемой строки*/
		*buffer++ = currChar;                   /* пишем символ в выходной буфер */
		if (currChar == '\n') {                 /*Анализируем, является ли символ разделителем*/
			*(buffer - 1) = '\0';               /*Да, вместо него ставим конец строки*/
			//recvn(sock, tempBuf, tmpcount - count + 1); //удаляем из потока символы вместе с разделителем
			return buffer - buff - 1;             /* возвращаем длину сообщения */
		}
	}
	return -1;                                  /*некорректный выход (если buffsize меньше или равен 1)*/
}

//Функция передачи данных (т.к. мы тоже можем передать не все сразу)
int sendn(SOCKET s, char* buf, int lenbuf, int flags) {
	int bytesSended = 0;                    //сколько всего прочитали
	int n;                                  // сколько прочитали за 1 вызов
	while (bytesSended < lenbuf) {
		n = send(s, buf + bytesSended, lenbuf - bytesSended, flags);
		if (n < 0) {
			printf("Error with send in sendn\n");
			break;
		}
		bytesSended += n;
	}
	return (n == -1 ? -1 : bytesSended);
}

//посылка сообщения с разделителями
int sendLine(SOCKET sock, char* str) {
	char tempBuf[MAX_STR_LEN];
	strcpy(tempBuf, str);                                       //формируем новую строку
	if (tempBuf[strlen(tempBuf) - 1] != '\n')
		strcat(tempBuf, "\n");                                  //вставляем разделитель
	return sendn(sock, tempBuf, strlen(tempBuf), 0); //отсылаем строку
}
