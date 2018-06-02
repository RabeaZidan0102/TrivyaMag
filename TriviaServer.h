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
	MT_CLIENT_LOG_IN = 200,
	MT_CLIENT_SIGN_UP = 202,
	MT_CLIENT_SIGN_OUT = 207,
	MT_CLIENT_LEAVE_GAME = 208,
	MT_CLIENT_START_GAME = 101,
	MT_CLIENT_PLAYERS_ANSWER = 111,
	MT_CLIENT_CREATE_ROOM = 244,
	MT_CLIENT_CLOSE_ROOM = 24,
	MT_CLIENT_JOIN_ROOM = 55,
	MT_CLIENT_LEAVE_ROOM = 5,
	MT_CLIENT_GET_USERS_IN_ROOM = 554,
	MT_CLIENT_GET_ROOMS = 54,
	MT_CLIENT_GET_BEST_SCORE = 54,
	MT_CLIENT_GET_PERSONAL_STATUS = 45,
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
	void acceptClient();
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
	
	void handleRecieveMessage();
	void addRecieveMessage(RecievedMessage* msg);
	RecievedMessage* buildRecieveMessage(SOCKET sock, int msgCode);

	User* getUserByName(string username);
	User* getUserBySocket(SOCKET sock);
	Room* getRoomByID(int id);



	// variables
	SOCKET _socket;
	map<SOCKET, User*> _connectedUsers;
	map<SOCKET, User*>::iterator usersItr;

	DataBase _db;

	map <int, Room*> _roomsList;
	map<int, Room*>::iterator roomsItr;

	mutex _mtxRecievedMessages;
	queue<RecievedMessage*> _queRcvMessages;
	condition_variable _msgCondition;
	condition_variable _edited;

	static int _roomIdSequence;
};
