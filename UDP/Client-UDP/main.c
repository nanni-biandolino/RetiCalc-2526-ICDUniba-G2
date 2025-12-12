 #if defined(WIN32)
    #include <winsock.h>
#else
    #define closesocket close
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <netdb.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXDIM 255
#define PORT 48000

void ErrorHandler(char *errorMessage) {
    printf("%s\n", errorMessage);
}

void ClearWinSock() {
#if defined(WIN32)
    WSACleanup();
#endif
}

int main() {

#if defined(WIN32)
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        printf("error at WSAStartup\n");
        return EXIT_FAILURE;
    }
#endif

    int sock;
    struct sockaddr_in sendAddr;
    struct sockaddr_in recvAddr;

    int recvSize;
    int recvAddrLen;
    char recvBuff[MAXDIM+1];
    char sendBuff[MAXDIM+1];
    int recvStrLen;
    int sendStrLen;

    char hostName[MAXDIM];
    printf("Inserisci hostname: ");
    scanf("%s", hostName);

    int port = PORT;
    printf("Inserisci porta (default %d): ", PORT);
    scanf("%d", &port);
    int c;
    while ((c = getchar()) != '\n' && c != EOF);

    struct hostent *host;
    host = gethostbyname(hostName);

    if (host == NULL) {
        fprintf(stderr, "gethostbyname() failed.\n");
        exit(EXIT_FAILURE);
    } else {
        printf("Risultato gethostbyname(%s) = %s\n",
               hostName,
               inet_ntoa(*(struct in_addr*) host->h_addr_list[0]));
    }

    // CREAZIONE SOCKET
    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        ErrorHandler("socket() failed");

    // COSTRUZIONE INDIRIZZO SERVER
    memset(&sendAddr, 0, sizeof(sendAddr));
    sendAddr.sin_family = AF_INET;
    sendAddr.sin_port = htons(port);
    sendAddr.sin_addr = *(struct in_addr*) host->h_addr_list[0];

    // INVIO SALUTO
    strcpy(sendBuff, "Hello");
    sendStrLen = strlen(sendBuff);

    if (sendto(sock, sendBuff, sendStrLen, 0,
               (struct sockaddr*)&sendAddr, sizeof(sendAddr)) != sendStrLen)
        ErrorHandler("sendto() sent a different number of bytes");

    // INVIO MESSAGGIO
    printf("Inserisci la stringa da cui vuoi eliminare le vocali: ");
    fgets(sendBuff, sizeof(sendBuff), stdin);
    sendBuff[strcspn(sendBuff, "\n")] = '\0';
    sendStrLen = strlen(sendBuff);


    if (sendStrLen > MAXDIM)
        ErrorHandler("Stringa troppo lunga.");

    if (sendto(sock, sendBuff, sendStrLen, 0, (struct sockaddr*)&sendAddr, sizeof(sendAddr)) != sendStrLen)
        ErrorHandler("sendto() sent a different number of bytes");

    // RICEZIONE
    recvAddrLen = sizeof(recvAddr);
    recvStrLen = recvfrom(sock, recvBuff, MAXDIM, 0,
                          (struct sockaddr*)&recvAddr, &recvAddrLen);

    if (recvStrLen < 0) {
        ErrorHandler("recvfrom() failed");
        exit(EXIT_FAILURE);
    }

    if (sendAddr.sin_addr.s_addr != recvAddr.sin_addr.s_addr) {
        fprintf(stderr, "Error: received a packet from unknown source.\n");
        exit(EXIT_FAILURE);
    }

    recvBuff[recvStrLen] = '\0';

    host = gethostbyaddr(&recvAddr.sin_addr, sizeof(recvAddr.sin_addr),  AF_INET);
    char* serverName = host ? host->h_name : "Unknown";

    printf("Stringa \"%s\" ricevuta da server nome: \"%s\" indirizzo: %s\n",
           recvBuff, serverName, inet_ntoa(recvAddr.sin_addr));

    closesocket(sock);
    ClearWinSock();

    return EXIT_SUCCESS;
}
