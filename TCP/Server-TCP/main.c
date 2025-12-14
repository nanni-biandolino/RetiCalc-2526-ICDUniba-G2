#ifdef _WIN32
    #include <winsock.h>  
#else
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #define closesocket close
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define PORT 27015
#define BUFFERSIZE 512
#define QLEN 6

void ErrorHandler(const char *msg) {
    perror(msg);
}

void ClearWinSock() {
#ifdef _WIN32
    WSACleanup();
#endif
}

int filtroVocali(char *recvBuff, char* sendBuff, int recvSize){
    memset(sendBuff, 0, BUFFERSIZE);
    int k=0;
    for(int i=0; i<recvSize; i++){
        if(!(  recvBuff[i]=='a' ||
            recvBuff[i]=='e' ||
            recvBuff[i]=='i' ||
            recvBuff[i]=='o' ||
            recvBuff[i]=='u' ||
            recvBuff[i]=='A' ||
            recvBuff[i]=='E' ||
            recvBuff[i]=='I' ||
            recvBuff[i]=='O' ||
            recvBuff[i]=='U')
        )
        sendBuff[k++]=recvBuff[i];
    }
    sendBuff[k]='\0';
    return k;
}

int recive(int CSocket, char *buffer){
    int bytesRcvd = recv(CSocket, buffer, BUFFERSIZE - 1, 0);
    if (bytesRcvd <= 0) {
        printf("Client disconnected.\n");
    }else{
        buffer[bytesRcvd] = '\0';
    }
    return bytesRcvd;
}

/*int byteControl(int bytesRcvd, int CSocket){
     bytesRcvd = recv(CSocket, buffer, BUFFERSIZE - 1, 0);
    if (bytesRcvd <= 0) {
        printf("Client disconnected.\n");
        closesocket(CSocket);
    }
}*/

int main() {

#ifdef _WIN32
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(1,1), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup() failed\n");
        return -1;
    }
#endif

    // Creazione socket
    int WelcomeSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (WelcomeSocket < 0) {
        ErrorHandler("socket() failed");
        ClearWinSock();
        return -1;
    }

    // Indirizzo
    struct sockaddr_in sad;
    memset(&sad, 0, sizeof(sad));
    sad.sin_family = AF_INET;
    sad.sin_port = htons(PORT);

    sad.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (sad.sin_addr.s_addr == INADDR_NONE) {
        printf("Invalid address.\n");
        closesocket(WelcomeSocket);
        ClearWinSock();
        return -1;
    }

    // Bind
    if (bind(WelcomeSocket, (struct sockaddr*)&sad, sizeof(sad)) < 0) {
        ErrorHandler("bind() failed");
        closesocket(WelcomeSocket);
        ClearWinSock();
        return -1;
    }
    printf(" *** BIND *** \n");

    // Listen
    if (listen(WelcomeSocket, QLEN) < 0) {
        ErrorHandler("listen() failed");
        closesocket(WelcomeSocket);
        ClearWinSock();
        return -1;
    }
    printf(" *** IN ASCOLTO su 127.0.0.1 PORTA:%d *** \n ", PORT);

    struct sockaddr_in cad;
    int clientLen = sizeof(cad);

    while(1) {
        printf(" -- Aspetto un client... --\n");

        int CSocket = accept(WelcomeSocket, (struct sockaddr*)&cad, &clientLen);
        if (CSocket < 0) {
            ErrorHandler("accept() failed");
            continue;
        }

        printf(" -- CONNESSIONE con il client: %s --\n", inet_ntoa(cad.sin_addr));

        char buffer[BUFFERSIZE];
        int bytesRcvd;

        if((bytesRcvd=recive(CSocket, buffer))<=0){
            closesocket(CSocket);
            continue;
        }

        printf(" >> Ho ricevuto: \"%s\" da %s \n", buffer, inet_ntoa(cad.sin_addr));

        printf(" -- Aspetto una stringa da convertire ... -- \n");

        if((bytesRcvd=recive(CSocket, buffer))<=0){
            closesocket(CSocket);
            continue;
        }

        printf(" >> Ho ricevuto: \"%s\" da %s \n", buffer, inet_ntoa(cad.sin_addr));

        char sendBuff[BUFFERSIZE];
        int sendSize = filtroVocali(buffer, sendBuff, bytesRcvd);
        send(CSocket, sendBuff, sendSize, 0);
        printf(" << Inviato: %s\n", sendBuff);


        closesocket(CSocket);
        printf(" -- DISCONNESSIONE --\n   -- --- --- --- ---- --- --- --- -- \n");

    }
    closesocket(WelcomeSocket);
    ClearWinSock();
    return 0;
}

