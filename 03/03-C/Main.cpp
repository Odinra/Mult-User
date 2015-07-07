/******************************************************************************
******************************      CLIENT		*******************************
******************************************************************************/

#include <iostream>
#include <conio.h>
#include <WinSock2.h>




using namespace std;
HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
char ServerIP[15] = "161.109.16.137";
char SendData[100];
int SendBytes = 0;
char RecvData[240];
int RecvBytes= 0;
int iResult = 0;
char username[11];
char ch;
int serverSize = sizeof(sockaddr_in);


bool quit = true;
bool donerecieving = true;
bool donesending = true;

bool getData();
bool getUsername();
bool showData();

int main()
{

	cin.clear();
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
	skServer = socket(AF_INET, SOCK_DGRAM, 0);
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
		printf("SERVER TYPE:\tUDP...\n\n");
	}

	/**************************************************************************
	CONNECT TO SERVER FROM CLIENT
	**************************************************************************/
	//iResult = connect(skServer, (struct sockaddr*)&saServer, sizeof(sockaddr));
	//if(iResult == SOCKET_ERROR)
	//{
	//	printf("Connections failed: %i\n", WSAGetLastError());
	//	closesocket(skServer);
	//	WSACleanup();
	//	getch();
	//	return(0);
	//}

	/**************************************************************************
	SEND DATA FROM CLIENT TO SERVER
	**************************************************************************/
	do
	{
		printf("\nPlease enter the message you would like to send to the server\n(Press the ESCape key to exit):\n");
		do
		{
			sprintf(SendData, "\n<User: %s >", username);
			cin.clear();
			donesending = getData();
			SetConsoleTextAttribute(console, 3);
			printf("%s \n", SendData);
			SendBytes = sendto(skServer, SendData, sizeof(SendData), 0, (struct sockaddr*)&saServer, serverSize);
			//printf("\t\tSent %i bytes to server\n\n", SendBytes);
		}while(donesending);

		/**************************************************************************
		RECIEVE DATA FROM SERVER TO CLIENT
		**************************************************************************/
		if(quit)
		{
		do
		{
			SetConsoleTextAttribute(console, 4);
			printf("\n\n\n\n\t\t\t<Waiting for a Response>\n\n\n");
			if(RecvBytes == 0 || (RecvBytes == SOCKET_ERROR && WSAGetLastError()))
			{
				RecvBytes = recvfrom(skServer, RecvData, sizeof(RecvData), 0, (struct sockaddr*)&saServer, &serverSize);
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
			SetConsoleTextAttribute(console, 7);
			RecvBytes = 0;
		}while( donerecieving);
		}



	}while( quit  );

	/**************************************************************************
	CLOSE SOCKET WITH SERVER
	**************************************************************************/
	closesocket(skServer);
	WSACleanup();
	SetConsoleTextAttribute(console, 2);
	printf("\n\nConnection had been terminated...");

	SetConsoleTextAttribute(console, 8);
	cin.clear();
	printf("\n\n\n\nPress Any Key to Continue...");
	getch();

	return 0;
}

bool getData()
{
	SetConsoleTextAttribute(console, 7);
	int size;
	while((ch=getche())!='\r')
	{
		if(ch == 27)
		{
			size = sprintf(SendData, "%s\n\tUser Disconnected%c", SendData, 27);
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

	SetConsoleTextAttribute(console, 7);

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
	SetConsoleTextAttribute(console, 5);
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
	SetConsoleTextAttribute(console, 7);
}