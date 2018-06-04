#pragma once

#include <WinSock2.h>
#include <Windows.h>
#include <vector>
#include <thread>
#include <fstream>

#define FILE_NAME "shared_doc.txt"
#define BUFFER 99999
#define PORT 8876

using std::vector;
using std::string;
using std::fstream;
using std::ofstream;
using std::thread;

class Server
{
public:
	Server();
	~Server();
	void serve(int port);
	void clientsVectorChange();

private:
	void accept();
	void clientHandler(SOCKET clientSocket);
	void SendMessage101ToClient(string msgNumber, string fileSize, string fileContent,
		string firstUserNameSize, string fisrtUserName, string nextUserNameSize, string nextUserName, string position);
	void readingFile(string fileName);

	SOCKET _serverSocket;

	vector <string> _clientMsg;
	vector <string> _clientsVector;

	vector <SOCKET> _clientsSocketVector;
	vector <thread> _clientsThreadVector;
	
	string _101MsgToClient;
	string _fileContent;
	string fileSizeContent;
};

