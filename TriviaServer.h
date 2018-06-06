#pragma once

#include <WinSock2.h>
#include <Windows.h>
#include <string>
#include <iostream>
#include <map>
#include <mutex>
#include <queue>
#include "DataBase.h"
#include <condition_variable>
#include "Protocol.h"

#define PORT 8820 
#define BUFFER 4800
#define MT_CLIENT_EXIT 299
#define IFACE 0

using std::string;
using std::map;
using std::mutex;
using std::queue;
using std::condition_variable;

class RecievedMessage;
class User;
class Room;


enum MessageType : byte
{
	MT_CLIENT_SIGN_IN = 200,
	MT_CLIENT_SIGN_OUT = 201,
	MT_CLIENT_SIGN_UP = 203,
	MT_CLIENT_GET_ROOMS = 205,
	MT_CLIENT_GET_USERS_IN_ROOM = 207,
	MT_CLIENT_JOIN_ROOM = 209,
	MT_CLIENT_LEAVE_ROOM = 211,
	MT_CLIENT_CREATE_ROOM = 213,
	MT_CLIENT_CLOSE_ROOM = 215,
	MT_CLIENT_START_GAME = 217,
	MT_CLIENT_PLAYERS_ANSWER = 219,
	MT_CLIENT_LEAVE_GAME = 222,
	MT_CLIENT_GET_BEST_SCORE = 223,
	MT_CLIENT_GET_PERSONAL_STATUS = 225,
};


class TriviaServer
{
public:
	TriviaServer();
	~TriviaServer();
	
	void serve();


private:

	//functions
	void bindAndListen();
	void accept();
	void clientHandler(SOCKET sock);
	void safeDeleteUser(RecievedMessage* msg);

	User* handleSignin(RecievedMessage* msg);
	bool handleSignup(RecievedMessage* msg);
	void handleSignout(RecievedMessage* msg);

	void handleLeaveGame(RecievedMessage* msg);
	void handleStartGame(RecievedMessage* msg);
	void handlePlayerAnswer(RecievedMessage* msg);

	bool handleCreateRoom(RecievedMessage* msg);
	bool handleCloseRoom(RecievedMessage* msg);
	bool handleJoinRoom(RecievedMessage* msg);
	bool handleLeaveRoom(RecievedMessage* msg);
	void handleGetUserslnRoom(RecievedMessage* msg);
	void handleGetRooms(RecievedMessage* msg);

	void handleGetBestScore(RecievedMessage* msg);
	void handleGetPersonalStatus(RecievedMessage* msg);
	
	void handleRecievedMessage();
	void addRecieveMessage(RecievedMessage* msg);
	RecievedMessage* buildRecieveMessage(SOCKET sock, int msgCode);

	User* getUserByName(string username);
	User* getUserBySocket(SOCKET sock);
	Room* getRoomByID(int id);


	// variables
	SOCKET _socket;
	map<SOCKET, User*> _connectedUsers;
	map<SOCKET, User*>::iterator usersItr;
	mutex _mtxUsers;

	DataBase _db;

	map <int, Room*> _roomsList;
	map<int, Room*>::iterator roomsItr;

	mutex _mtxRecievedMessages;
	queue<RecievedMessage*> _queRcvMessages;
	condition_variable _msgCondition;
	condition_variable _edited;

	int _roomIdSequence;
	
	Protocol _Protocol;
};
