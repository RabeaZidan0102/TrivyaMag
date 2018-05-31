#pragma once

#include <WinSock2.h>
#include <Windows.h>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include "Helper.h"

using std::string;
using std::vector;
using std::stringstream;

class Room;
class User;
class Question;

// msg -> from the server
// msg -> from the client

class Protocol
{
public:
	Protocol(SOCKET sock);
	~Protocol();
	/*
	//requests
	string request200(string username, string password);
	string request201();
	string request203(string username, string password, string email);
	string request205();
	string request207(int roomID);
	string request209(int roomID);
	string request211();
	string request213(string roomName, int playersNumber, int questionsNumber, int time);
	string request215();
	string request217();
	string request219(int numberOfQuestion, int timeInSeconds);
	string request222();
	string request223();
	string request225();
	*/

	//responses
	void response102(string details); // 0 or 1 or 2
	void response104(string respond);
	void response106(vector<Room*> rooms);
	void response108(vector<User*> users, bool error);
	void response110(int questionsNumber, int time); // 0 /1/2
	void response112();
	void response114(int status); // 0 fail, 1 success
	void response116(vector<User*> users);
	void response118(Question* question);
	void response120(int yesOrNot);
	void response121(vector<User*> users);
	void response124(vector<User*> users);
	void response126(vector<User*> users);


private:
	Helper _myHelper;
	SOCKET _socket;
};
