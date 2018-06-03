#include "TriviaServer.h"
#include "Game.h"
#include "User.h"
#include "RecievedMessage.h"
#include "Room.h"
#include "Question.h"
#include <exception>
#include <thread>
#include <mutex>
#include <vector>

using std::exception;
using std::lock_guard;

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
	std::thread tr(&TriviaServer::handleRecievedMessage, this);
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
	TRACE("binded");

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
	int msgCode = Helper::getMessageTypeCode(sock);

	RecievedMessage* theReturnedMessage;
	try
	{
		while (msgCode != 0 || msgCode != MT_CLIENT_EXIT)
		{
			theReturnedMessage = this->buildRecieveMessage(sock, msgCode);
			this->addRecieveMessage(theReturnedMessage);
			int newMsgCode = Helper::getMessageTypeCode(sock);
		}
	}
	catch (exception &e)
	{
		cout << e.what() << endl;
		RecievedMessage* errorRcvMsg = new RecievedMessage(sock, 299);
		this->addRecieveMessage(errorRcvMsg);
		closesocket(sock);
	}
}

User * TriviaServer::handleSignin(RecievedMessage * msg)
{
	return nullptr;
}

void TriviaServer::handleRecievedMessage()
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
	string msgInString = Helper::getStringPartFromSocket(sock, BUFFER);
	vector<string> parameters;
	
	if (msgCode == MT_CLIENT_SIGN_IN)
	{
		parameters.push_back(msgInString.substr(5, std::stoi(msgInString.substr(3, 2))));
		parameters.push_back(msgInString.substr(5 + std::stoi(msgInString.substr(3, 2)) + 2, std::stoi(msgInString.substr(std::stoi(msgInString.substr(3, 2)) + 5, 2))));

		RecievedMessage * msg = new RecievedMessage(sock, msgCode, parameters);
		parameters.clear();
		this->handleSignin(msg);
		return msg;
	}
	else if (msgCode == MT_CLIENT_SIGN_OUT)
	{
		RecievedMessage * msg = new RecievedMessage(sock, msgCode);
		this->handleSignout(msg);
		return msg;
	}
	else if (msgCode == MT_CLIENT_SIGN_UP)
	{
		parameters.push_back(msgInString.substr(5, std::stoi(msgInString.substr(3, 2))));
		parameters.push_back(msgInString.substr(5 + std::stoi(msgInString.substr(3, 2)) + 2, std::stoi(msgInString.substr(std::stoi(msgInString.substr(3, 2)) + 5, 2))));
		int up_till_name = std::stoi(msgInString.substr(3, 2));
		int up_till_pass = std::stoi(msgInString.substr(std::stoi(msgInString.substr(3, 2)) + 5, 2));
		parameters.push_back(msgInString.substr(5 + up_till_name + 2 + up_till_pass + 2, std::stoi(msgInString.substr(5 + up_till_name + 2 + up_till_pass, 2))));
		RecievedMessage * msg = new RecievedMessage(sock, msgCode, parameters);
		parameters.clear();
		this->handleSignup(msg);
		return msg;
	}
	else if (msgCode == MT_CLIENT_GET_ROOMS)
	{
		RecievedMessage * msg = new RecievedMessage(sock, msgCode);
		this->handleGetRooms(msg);
		return msg;
	}
	else if (msgCode == MT_CLIENT_GET_USERS_IN_ROOM)
	{
		parameters.push_back(msgInString.substr(3, 4));
		RecievedMessage * msg = new RecievedMessage(sock, msgCode, parameters);
		parameters.clear();
		this->handleGetUserslnRoom(msg);
		return msg;
	}
	else if (msgCode == MT_CLIENT_JOIN_ROOM)
	{
		parameters.push_back(msgInString.substr(3, 4));
		RecievedMessage * msg = new RecievedMessage(sock, msgCode, parameters);
		parameters.clear();
		this->handleJoinRoom(msg);
		return msg;
	}
	else if (msgCode == MT_CLIENT_LEAVE_ROOM)
	{
		RecievedMessage * msg = new RecievedMessage(sock, msgCode);
		this->handleLeaveGame(msg);
		return msg;
	}
	else if (msgCode == MT_CLIENT_CREATE_ROOM)
	{
		int sizeOfName = std::stoi(msgInString.substr(3, 2));
		int indexOfName = 5;
		int indexOfPlayersNumber = 5 + sizeOfName;
		int indexOfQuestionsNumber = indexOfPlayersNumber + 1;
		int indexOfQuestionTime = indexOfQuestionsNumber + 2;

		parameters.push_back(msgInString.substr(indexOfName, sizeOfName));
		parameters.push_back(msgInString.substr(indexOfPlayersNumber, 1));
		parameters.push_back(msgInString.substr(indexOfQuestionsNumber, 2));
		parameters.push_back(msgInString.substr(indexOfQuestionTime, 2));

		RecievedMessage * msg = new RecievedMessage(sock, msgCode, parameters);
		parameters.clear();
		this->handleCreateRoom(msg);
		delete msg;
	}
	else if (msgCode == MT_CLIENT_CLOSE_ROOM)
	{
		RecievedMessage * msg = new RecievedMessage(sock, msgCode);

		this->handleCloseRoom(msg);
		return msg;
	}
	else if (msgCode == MT_CLIENT_START_GAME)
	{
		RecievedMessage * msg = new RecievedMessage(sock, msgCode);

		this->handleStartGame(msg);
		return msg;
	}
	else if (msgCode == MT_CLIENT_PLAYERS_ANSWER)
	{
		parameters.push_back(std::to_string(msgInString[3]));
		parameters.push_back(msgInString.substr(5, 2));
		RecievedMessage * msg = new RecievedMessage(sock, msgCode, parameters);
		parameters.clear();
		this->handlePlayerAnswer(msg);
		return msg;
	}
	else if(msgCode == MT_CLIENT_LEAVE_GAME)
	{
		RecievedMessage * msg = new RecievedMessage(sock, msgCode);
		
		this->handleLeaveGame(msg);
		return msg;
	}
	else if(msgCode == MT_CLIENT_GET_BEST_SCORE)
	{
		RecievedMessage * msg = new RecievedMessage(sock, msgCode);
		
		handleGetBestScore(msg);
		return msg;
	}
	else if( msgCode == MT_CLIENT_GET_PERSONAL_STATUS)
	{
		RecievedMessage * msg = new RecievedMessage(sock, msgCode);
		
		handleGetPersonalStatus(msg);
		return msg;
	}
	
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
