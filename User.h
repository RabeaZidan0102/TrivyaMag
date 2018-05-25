#pragma once

#include <WinSock2.h>
#include <Windows.h>
#include <string>
#include <iostream>
#include "Helper.h"
#include "Room.h"
#include "Game.h"

using std::string;

class User
{
public:
	User(string username, SOCKET socket);
	~User();
	void send(string message);
	string getUsername();
	SOCKET getSocket();
	Room* getRoom();
	Game* getGame();
	void setGame(Game* gm);
	void clearRoom();
	bool createRoom(int roomID, string roomName, int maxUsers, int questionNumber, int questionTime);
	bool joinRoom(Room* newRoom);
	void leaveRoom();
	int closeRoom();
	bool leaveGame();

private:
	string _username;
	Room* _currRoom;
	Game* _currGame;
	SOCKET _sock;
	Helper _Helper;
};
