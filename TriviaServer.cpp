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
#include "Protocol.h"

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

void TriviaServer::safeDeleteUser(RecievedMessage * msg)
{
	try
	{
		SOCKET sock= msg->getSock();
		this->handleSignout(msg);
		closesocket(sock);
	}
	catch (exception& e)
	{
		cout << e.what() << endl;
	}
}

User * TriviaServer::handleSignin(RecievedMessage * msg)
{
	try
	{
		vector<string> details = msg->getValues();
		string username = details[0];
		string password = details[1];

		if (_db.isUserAndPassMatch(username, password))
		{
			if (this->getUserByName(username))
			{
				_Protocol.response102("2", msg->getSock()); // user is already connected
			}
			else
			{
				User* newUser = new User(username, msg->getSock());
				_mtxUsers.lock();
				_connectedUsers.insert(pair<SOCKET, User*> (msg->getSock(), newUser));
				_mtxUsers.unlock();
				_Protocol.response102("0", msg->getSock()); // success
				return newUser;
			}
		}
		else
		{
			_Protocol.response102("1", msg->getSock()); // wrong details
		}

	}
	catch (exception& e)
	{
		cout << e.what() << endl;
	}

	return nullptr;
}

void TriviaServer::handleSignout(RecievedMessage * msg)
{
	try
	{
		if (this->getUserBySocket(msg->getSock()))
		{
			usersItr = _connectedUsers.find(msg->getSock());
			_connectedUsers.erase(usersItr);
			this->handleCloseRoom(msg);
			this->handleLeaveRoom(msg);
			this->handleLeaveGame(msg);
		}
	}
	catch (exception& e)
	{
		cout << e.what() << endl;
	}
}

void TriviaServer::handleRecievedMessage()
{
	std::unique_lock<mutex> locker(_mtxRecievedMessages);
	while (true)
	{
		if (_queRcvMessages.empty())
		{
			_msgCondition.wait(locker);
		}

		RecievedMessage* rcvMsg = _queRcvMessages.front();

		try
		{
			switch (rcvMsg->getMessageCode())
			{
			case MT_CLIENT_SIGN_IN:
				this->handleSignin(rcvMsg);
				break;

			case MT_CLIENT_SIGN_OUT:
				this->handleSignout(rcvMsg);
				break;

			case MT_CLIENT_SIGN_UP:
				this->handleSignup(rcvMsg);
				break;

			case MT_CLIENT_GET_ROOMS:
				this->handleGetRooms(rcvMsg);
				break;

			case MT_CLIENT_GET_USERS_IN_ROOM:
				this->handleGetUserslnRoom(rcvMsg);
				break;

			case MT_CLIENT_JOIN_ROOM:
				this->handleJoinRoom(rcvMsg);
				break;

			case MT_CLIENT_LEAVE_ROOM:
				this->handleLeaveRoom(rcvMsg);
				break;

			case MT_CLIENT_CREATE_ROOM:
				this->handleCreateRoom(rcvMsg);
				break;

			case MT_CLIENT_CLOSE_ROOM:
				this->handleCloseRoom(rcvMsg);
				break;

			case MT_CLIENT_START_GAME:
				this->handleStartGame(rcvMsg);
				break;

			case MT_CLIENT_PLAYERS_ANSWER:
				this->handlePlayerAnswer(rcvMsg);
				break;

			case MT_CLIENT_LEAVE_GAME:
				this->handleLeaveGame(rcvMsg);
				break;

			case MT_CLIENT_GET_BEST_SCORE:
				this->handleGetBestScore(rcvMsg);
				break;

			case MT_CLIENT_GET_PERSONAL_STATUS:
				this->handleGetPersonalStatus(rcvMsg);
				break;

			default:
				this->safeDeleteUser(rcvMsg);
			}
		}
		catch (exception& e)
		{
			cout << e.what() << endl;
			this->safeDeleteUser(rcvMsg);
		}
	}
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
		return msg;
	}
	else if (msgCode == MT_CLIENT_SIGN_OUT)
	{
		RecievedMessage * msg = new RecievedMessage(sock, msgCode);
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

		return msg;
	}
	else if (msgCode == MT_CLIENT_GET_ROOMS)
	{
		RecievedMessage * msg = new RecievedMessage(sock, msgCode);
		return msg;
	}
	else if (msgCode == MT_CLIENT_GET_USERS_IN_ROOM)
	{
		parameters.push_back(msgInString.substr(3, 4));
		RecievedMessage * msg = new RecievedMessage(sock, msgCode, parameters);
		parameters.clear();
		return msg;
	}
	else if (msgCode == MT_CLIENT_JOIN_ROOM)
	{
		parameters.push_back(msgInString.substr(3, 4));
		RecievedMessage * msg = new RecievedMessage(sock, msgCode, parameters);
		parameters.clear();
		return msg;
	}
	else if (msgCode == MT_CLIENT_LEAVE_ROOM)
	{
		RecievedMessage * msg = new RecievedMessage(sock, msgCode);
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
		return msg;
	}
	else if (msgCode == MT_CLIENT_CLOSE_ROOM)
	{
		RecievedMessage * msg = new RecievedMessage(sock, msgCode);
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
		return msg;
	}
	else if(msgCode == MT_CLIENT_LEAVE_GAME)
	{
		RecievedMessage * msg = new RecievedMessage(sock, msgCode);
		return msg;
	}
	else if(msgCode == MT_CLIENT_GET_BEST_SCORE)
	{
		RecievedMessage * msg = new RecievedMessage(sock, msgCode);
		return msg;
	}
	else if( msgCode == MT_CLIENT_GET_PERSONAL_STATUS)
	{
		RecievedMessage * msg = new RecievedMessage(sock, msgCode);
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
