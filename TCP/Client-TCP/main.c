#ifdef WIN32
    #include <winsock.h>
#else
    #define closesocket close
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <unistd.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#define BUFFERSIZE 512


void ErrorHandler(char *errorMessage) {
    printf ("%s", errorMessage);
}

void ClearWinSock() {
     #if defined WIN32
     WSACleanup();
     #endif
}

int main(void)
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
     int CSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
     if (CSocket < 0) {  //errore nella creazione della socket
        ErrorHandler("socket creation failed.\n");
        ClearWinSock();
        return -1;
     }

    // COSTRUZIONE INDIRIZZO DEL SERVER
    struct sockaddr_in sad;
    memset(&sad, 0, sizeof(sad)); 
    sad.sin_family = AF_INET;
    sad.sin_addr.s_addr = inet_addr("127.0.0.1");
    sad.sin_port = htons(27015); 

    printf(" FANTASTIC PROGRAM that REMOVE VOCALS\n\n");
    system("pause");

    int acceptedConnection=0;

     // CONNESSIONE AL SERVER

     if (connect(CSocket, (struct sockaddr *)&sad, sizeof(sad)) < 0)
     {
         ErrorHandler( "Failed to connect.\n" );
         closesocket(CSocket);
         ClearWinSock();
         return -1;
     }else{
        printf(" -- Extablishing connection with server %s on Port %d --\n", inet_ntoa(sad.sin_addr), ntohs(sad.sin_port) );
     }


    // PREPARAZIONE INVIO

    char inputString[512];
    strcpy(inputString, "Hello");
    int stringLen = strlen(inputString);

    // INVIO "HELLO"

    if (send(CSocket, inputString, stringLen, 0) != stringLen) {
        ErrorHandler("send() sent a different number of bytes than expected");
        closesocket(CSocket);
        ClearWinSock();
        return -1;
    }else{
        printf(" << Sent : \"%s\" --\n", inputString);
    }

    //INPUT STRINGA DA CONVERTIRE

    printf("\n -- Write a message please >> ");
    fgets(inputString, sizeof(inputString), stdin);
    inputString[strcspn(inputString, "\n")] = '\0';
    stringLen = strlen(inputString);

    // INVIO STRINGA DA CONVERTIRE

    if (send(CSocket, inputString, stringLen, 0) != stringLen) {
        ErrorHandler("send() sent a different number of bytes than expected");
        closesocket(CSocket);
        ClearWinSock();
        return -1;
    }else{
        printf(" << Sent : \"%s\" --\n", inputString);
    }

    // PREPARAZIONE RICEZIONE

    char buffer[BUFFERSIZE];

    // ricezione risposta dal server
    int bytesRcvd = recv(CSocket, buffer, BUFFERSIZE-1, 0);
    if (bytesRcvd > 0) {
        buffer[bytesRcvd] = '\0';
        printf(" >> Recived : %s\n", buffer);
    }

    closesocket(CSocket);
    ClearWinSock();
    return 0;
}

