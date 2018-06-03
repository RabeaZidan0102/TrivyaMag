#include "TriviaServer.h"
#include "Game.h"
#include "User.h"
#include "RecievedMessage.h"
#include "Room.h"
#include "Question.h"
#include <exception>
#include <thread>
#include <mutex>

static const int PORT = 8820;

using std::exception;
using std::lock_guard;
using std::thread;

TriviaServer::TriviaServer()
{
	DataBase* temp = new DataBase();
	_db = *temp;

	try
	{
		_socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		if (_socket == INVALID_SOCKET)
		{
			throw std::exception(__FUNCTION__ " - socket");
		}
	}
	catch (exception& e)
	{
		cout << e.what() << endl;
	}
}

TriviaServer::~TriviaServer()
{
	_roomsList.clear();
	_connectedUsers.clear();
	
	try
	{
		// the only use of the destructor should be for freeing 
		// resources that was allocated in the constructor
		::closesocket(_socket);
	}
	catch (...) {}
}

void TriviaServer::serve()
{
	this->bindAndListen();

	//create new thread for handling message
	std::thread tr(&TriviaServer::handleRecieveMessage, this);
	tr.detach();

	while (true)
	{
		TRACE("accepting client...");
		this->acceptClient();
	}
}

void TriviaServer::bindAndListen()
{
	struct sockaddr_in sa = { 0 };
	sa.sin_port = htons(PORT);
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = INADDR_ANY;
	// again stepping out to the global namespace
	if (::bind(_socket, (struct sockaddr*)&sa, sizeof(sa)) == SOCKET_ERROR)
		throw std::exception(__FUNCTION__ " - bind");

	if (::listen(_socket, SOMAXCONN) == SOCKET_ERROR)
		throw std::exception(__FUNCTION__ " - listen");
	cout << "Listening on port " << PORT << endl;
}

void TriviaServer::acceptClient()
{
	SOCKET client_socket = accept(_socket, NULL, NULL);
	if (client_socket == INVALID_SOCKET)
		throw std::exception(__FUNCTION__);

	TRACE("Client accepted !");
	// create new thread for client	and detach from it
	std::thread tr(&TriviaServer::clientHandler, this, client_socket);
	tr.detach();
}

void TriviaServer::clientHandler(SOCKET sock)
{
	try
	{
		int msgFromClient = Helper::getMessageTypeCode(sock);
		
		while (msgFromClient != MT_CLIENT_EXIT || msgFromClient != 0)
		{
			this->buildRecieveMessage(sock, msgFromClient);
		}
	}
	catch (exception& e)
	{
		cout << e.what() << endl;
	}
}

void TriviaServer::handleRecieveMessage()
{

}

void TriviaServer::addRecieveMessage(RecievedMessage * msg)
{
	lock_guard<mutex> lck(_mtxRecievedMessages);
	_queRcvMessages.push(msg);
	lck.~lock_guard();
	_msgCondition.notify_all();
}

RecievedMessage * TriviaServer::buildRecieveMessage(SOCKET sock, int msgCode)
{

	return nullptr;
}

User * TriviaServer::getUserByName(string username)
{
	for (usersItr = _connectedUsers.begin(); usersItr != _connectedUsers.end(); ++usersItr)
	{
		if (usersItr->second->getUsername() == username)
		{
			return (usersItr->second);
		}
	}

	return nullptr;
}

User * TriviaServer::getUserBySocket(SOCKET sock)
{
	for (usersItr = _connectedUsers.begin(); usersItr != _connectedUsers.end(); ++usersItr)
	{
		if (usersItr->first == sock)
		{
			return (usersItr->second);
		}
	}

	return nullptr;
}

Room * TriviaServer::getRoomByID(int id)
{
	for (roomsItr = _roomsList.begin(); roomsItr != _roomsList.end(); ++roomsItr)
	{
		if (roomsItr->first == id)
		{
			return _roomsList[id];
		}
	}
	return nullptr;
}
