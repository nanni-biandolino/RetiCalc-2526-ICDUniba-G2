#ifdef WIN32
    #include <winsock.h>
#else
    #define closesocket close
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <unistd.h>
#endif
#include <stdio.h>
#include <stdlib.h> // for atoi()
#define BUFFDIM 255
#define PORT 48000

//stampa messaggi errore
void ErrorHandler(char *errorMessage) {
    printf ("%s", errorMessage);
}

//chiusura socket
void ClearWinSock() {
     #if defined WIN32
     WSACleanup();
     #endif
}

int filtroVocali(char *recvBuff, char* sendBuff, int recvSize){
    memset(sendBuff, 0, BUFFDIM);
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

int main(int argc, char *argv[])
{
    #ifdef WIN32
    // Initialize Winsock
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2,2),&wsaData);
    if(iResult != 0) {
        printf("Error at WSAStartup()\n");
        return 0;
    }
    #endif

    int sock;
    struct sockaddr_in sad; //IP + PORTA SERVER
    struct sockaddr_in cad; //IP + PORTA CLIENT
    unsigned int cliAddrLen;
    char recvBuff[BUFFDIM];
    char sendBuff[BUFFDIM];
    int recvSize;
    int sendSize;


    // COSTRUZIONE DELL'INDIRIZZO DEL SERVER
    memset(&sad, 0, sizeof(sad));
    sad.sin_family = AF_INET;
    sad.sin_port = htons(PORT);
    sad.sin_addr.s_addr = inet_addr("127.0.0.1");

     // CREAZIONE DELLA SOCKET
    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        ErrorHandler("socket() failed");
    else{
        printf("1) Socket created \n");
    }


    // BIND DELLA SOCKET
    if ((bind(sock, (struct sockaddr *)&sad, sizeof(sad))) < 0){
        ErrorHandler("bind() failed");
    }else{
        printf("2) Binding executed on 127.0.0.1:%d \n   .....  .....  ..... .....   \n",ntohs(sad.sin_port));
    }


    // RICEZIONE DELLA STRINGA  DAL CLIENT
    while(1) {
        cliAddrLen = sizeof(cad);
        recvSize = recvfrom(sock, recvBuff, BUFFDIM, 0, (struct sockaddr*)&cad, &cliAddrLen);
        if(recvSize<0)continue;
        recvBuff[recvSize] = '\0';
        struct hostent *host = gethostbyaddr(&cad.sin_addr, sizeof(cad.sin_addr),  AF_INET);
        char* clientName = host ? host->h_name : "Unknown";
        printf("3) Ricevuti dati dal client nome: %s, indirizzo: %s, >> %s\n", clientName, inet_ntoa(cad.sin_addr), recvBuff);



        if(strcmp(recvBuff, "Hello")){


            recvSize = filtroVocali(recvBuff, sendBuff, recvSize);

            // RINVIA LA STRINGA MODIFICATA AL CLIENT
            sendSize = sendto(sock, sendBuff, recvSize, 0, (struct sockaddr *)&cad, sizeof(cad));
            if (sendSize != recvSize){
                ErrorHandler("sendto() sent different number of bytes than expected");
            }
        }
    }
 }
