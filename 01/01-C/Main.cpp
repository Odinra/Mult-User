/******************************************************************************
******************************      CLIENT		*******************************
******************************************************************************/

#include <iostream>
#include <conio.h>
#include <WinSock2.h>

using namespace std;
	
char ServerIP[15] = "127.0.0.1";
char SendData[100];
int SendBytes = 0;
char RecvData[240];
int RecvBytes= 0;
int iResult = 0;
char username[11];
char ch;

void getData();

int main()
{


	printf("Please enter a UserName (10 characters or less)");
	fgets(username, 10, stdin);

	int numberMessages = 0;


/**************************************************************************
SETUP SOCKET VARIABLES AN OBJECTS
**************************************************************************/
	SOCKET skServer;
	sockaddr_in saServer;
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
		printf("SOCKET:\topen...\n");
		printf("SERVER TYPE:\tTCP...\n\n");
	}

/**************************************************************************
CONNECT TO SERVER FROM CLIENT
**************************************************************************/
	iResult = connect(skServer, (struct sockaddr*)&saServer, sizeof(sockaddr));
	if(iResult == SOCKET_ERROR)
	{
		printf("Connections failed: %i\n", WSAGetLastError());
		closesocket(skServer);
		WSACleanup();
		getch();
		return(0);
	}
	printf("Please enter the message you would like to send to the server\n(Press the ESCape key to exit):\n");
/**************************************************************************
SEND DATA FROM CLIENT TO SERVER
**************************************************************************/
	do
	{

	getData();

	//printf("SENDING DATA: \t%s \n", SendData);
	SendBytes = send(skServer, SendData, sizeof(SendData), 0);
	//printf("\t\tSent %i bytes to server\n\n", SendBytes);

/**************************************************************************
RECIEVE DATA FROM SERVER TO CLIENT
**************************************************************************/

	if(RecvBytes == 0 || (RecvBytes == SOCKET_ERROR && WSAGetLastError()))
	{
		RecvBytes = recv(skServer, RecvData, sizeof(RecvData), 0);
		if(RecvBytes == -1)
		{
			//printf("Connection closed.\n");
			return 0;
		}
		if(RecvBytes < 0)
			return 0;
			//printf("RECEIVING DATA:\t%s\n", RecvData);
			//printf("\t\tRecieved %i bytes from Server\n\n", RecvBytes);
	}

	numberMessages++;
	RecvBytes = 0;
	}

	

	while( true  );

/**************************************************************************
CLOSE SOCKET WITH SERVER
**************************************************************************/
			closesocket(skServer);
			WSACleanup();
			printf("\n\nConnection had been terminated...");

	printf("\n\n\n\nPress Any Key to Continue...");
	getch();

	return 0;
}

void getData()
{
	while((ch=getche())!='\r')
	{

	}
}