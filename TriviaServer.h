#pragma once

#include <WinSock2.h>
#include <Windows.h>
#include <string>
#include <iostream>
#include <map>
#include <mutex>
#include <queue>
#include "DataBase.h"

using std::string;
using std::map;
using std::mutex;
using std::queue;

class RecievedMessage;
class User;
class Room;

class TriviaServer
{
public:
	TriviaServer();
	~TriviaServer();
	
	void server();


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
	
	void handleRecieveMessage();
	void addRecieveMessage(RecievedMessage* msg);
	RecievedMessage* buildRecieveMessage(SOCKET sock, int msgCode);

	User* getUserByName(string username);
	User* getUserBySocket(SOCKET sock);
	Room* getRoomByID(int id);

	// variables
	SOCKET _socket;
	map<SOCKET, User*> _connectedUsers;
	DataBase _db;
	map <int, Room*> _roomsList;

	mutex _mtxRecievedMessages;
	queue<RecievedMessage*> _queRcvMessages;

	static int _roomIdSequence;
};
