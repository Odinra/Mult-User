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

bool quit = true;
bool donerecieving = true;
bool donesending = true;

bool getData();
bool getUsername();
bool showData();

int main()
{


	printf("Please enter a UserName (10 characters or less)\n");
	getUsername();

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
	
/**************************************************************************
SEND DATA FROM CLIENT TO SERVER
**************************************************************************/
	do
	{
		printf("\nPlease enter the message you would like to send to the server\n(Press the ESCape key to exit):\n");
		do
		{
			sprintf(SendData, "\n<User: %s >", username);

			donesending = getData();

			printf("%s \n", SendData);
			SendBytes = send(skServer, SendData, sizeof(SendData), 0);
		//printf("\t\tSent %i bytes to server\n\n", SendBytes);
		}while(donesending);

/**************************************************************************
RECIEVE DATA FROM SERVER TO CLIENT
**************************************************************************/
	do
	{
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
		
		donerecieving = showData();
			//printf("%s\n", RecvData);
			//printf("\t\tRecieved %i bytes from Server\n\n", RecvBytes);
	}

	RecvBytes = 0;
	}while( donerecieving);
	

	

		}while( quit  );

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

bool getData()
{
	int size;
	while((ch=getche())!='\r')
	{
		if(ch == 27)
		{
			quit = false;
			return false;
		}
		
		size = sprintf(SendData, "%s%c", SendData, ch);
		
		if(size >= 50 && ch == ' ')
		{
			size = sprintf(SendData, "%s%c", SendData, '\0');
			return true;
			
		}
		
		if(size > 65)
		{
			size = sprintf(SendData, "%s%c", SendData, '\0');
			return true;
			
		}
	}

	sprintf(SendData, "%s%c", SendData, 7);

	return false;
}

bool getUsername()
{
	int size;
	while((ch=getche())!='\r')
	{
		if(ch == 27)
			return false;
		
		size = sprintf(username, "%s%c", username, ch);
		
		if(size >= 10)
			return true;
	}

	return true;
}

bool showData()
{
	char charecter;
	for(int i = 0; i < RecvBytes; i++)
	{
		charecter = RecvData[i];

		if (charecter == 27)
		{
			quit = false;
			return false;
		}

		else if (charecter == 7)
			return false;

		else if (charecter == '\0')
			return true;

		else
			printf("%c", charecter);
	}
}