/******************************************************************************
******************************      SERVER		*******************************
******************************************************************************/
#include <iostream>
#include <conio.h>
#include <WinSock2.h>

using namespace std;


struct IPAddress
{
	int seg1, seg2, seg3, seg4;
};

int main()
{
	char ServerIP[15] = "127.0.0.1";
	IPAddress ClientIP;
	char SendData[100];
	int SendBytes = 0;
	char RecvData[240];
	int RecvBytes= 0; 
	int clientSize = sizeof(sockaddr_in);
	int iResult = 0;
	char username[11];	
	int numberMessages = 0;

	printf("Please enter a UserName (10 characters or less)");
	fgets(username, 10, stdin);

/**************************************************************************
SETUP SOCKET VARIABLES AN OBJECTS
**************************************************************************/

	SOCKET skServer;
	SOCKET skClient;
	sockaddr_in saServer;
	sockaddr_in saClient;
	WSADATA wsaData;
	iResult = WSAStartup(MAKEWORD(2, 0), &wsaData);



	if (iResult != NO_ERROR)
	{
		printf("Error with WSAStartup()\n");
		return(0);
	}

/**************************************************************************
CLEAR MEMORY AREA - SET ALL TO ZERO (0)
**************************************************************************/
	memset(RecvData, 0, sizeof(RecvData));
	memset(&saServer, 0, sizeof(sockaddr_in));

/**************************************************************************
SETUP SOCKET ADDRESS INFORMATION
**************************************************************************/
	saServer.sin_family = AF_INET;
	saServer.sin_addr.s_addr = inet_addr(ServerIP);
	saServer.sin_port = htons(2006);

/**************************************************************************
OPEN SOCKET FOR CONNECTION
**************************************************************************/
	skServer = socket(AF_INET, SOCK_STREAM, 0);
	if(skServer == INVALID_SOCKET)
	{
		printf("ERROR with socket():%i\n", WSAGetLastError());
		WSACleanup();
		getch();
		return(0);
	}
	else
	{
		printf("SERVER SOCKET:\topen...\n");
		printf("SERVER TYPE:\tTCP...\n\n");
	}

/**************************************************************************
BIND SERVER FOR CONNECTION
**************************************************************************/
	iResult = bind(skServer, (struct sockaddr*)&saServer, sizeof(sockaddr));
	if(iResult == SOCKET_ERROR)
	{
		printf("Error binding socket(): %i\n", WSAGetLastError());
		closesocket(skServer);
		WSACleanup();
		getch();
		return(0);
	}
	else
	{
		printf("BIND SERVER: Socket Bound\n");
		if(listen(skServer, SOMAXCONN) == SOCKET_ERROR)
		{
			printf("Error listening on socket(): %i\n", WSAGetLastError());
			closesocket(skServer);
			WSACleanup();
			getch();
			return(0);
		}
		else
		{
			printf("Listening on port 2006\n\n");
/**************************************************************************
Accept connection from client on server
**************************************************************************/
			skClient= accept(skServer, (struct sockaddr*)&saClient, &clientSize);

/**************************************************************************
Fill th structure with the IP information
**************************************************************************/
			ClientIP.seg1 = saClient.sin_addr.S_un.S_un_b.s_b1;
			ClientIP.seg2 = saClient.sin_addr.S_un.S_un_b.s_b2;
			ClientIP.seg3 = saClient.sin_addr.S_un.S_un_b.s_b3;
			ClientIP.seg4 = saClient.sin_addr.S_un.S_un_b.s_b4;

			printf("CONNECTION:\testablished wuth client...\n\n");
			do
			{	
			printf("Attempting to Recieve Packet...\n\n\n");
			RecvBytes = recv(skClient, RecvData, sizeof(RecvData), 0);

/**************************************************************************
RECIEVE DATA FROM CLIENT TO SERVER
**************************************************************************/


			printf("RECEIVING DATA:\t%s\n", RecvData);
			printf("\t\tRecieved %i bytes from client at IP: %i.%i.%i.%i\n\n", RecvBytes, ClientIP.seg1, ClientIP.seg2, ClientIP.seg3, ClientIP.seg4);

/**************************************************************************
SEND DATA FROM SERVER TO CLIENT
**************************************************************************/
			printf("Please enter the message you would like to send back to the client:\t");
	
			fgets(SendData,81, stdin);

			printf("SENDING DATA:\t%s\n", SendData);
			SendBytes = send(skClient, SendData, sizeof(SendData), 0);
			printf("\t\tSent %i bytes to client\n\n", SendBytes);
			

			numberMessages++; 
			} while(numberMessages < 4);
			
			printf("Disconnecting Connection...");
/**************************************************************************
CLOSE SOCKET WITH SERVER
**************************************************************************/
			closesocket(skServer);
			closesocket(skClient);
			WSACleanup();
			printf("\n\nConnection had been terminated...");			
		}
	}
	printf("\n\n\n\nPress Any Key to Continue...");
	getch();

	return 0;
}
