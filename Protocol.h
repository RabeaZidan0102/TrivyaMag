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
class DataBase;

// msg -> from the server
// msg -> from the client

class Protocol
{
public:
	Protocol();
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
	void response102(string details, SOCKET _socket); // 0 or 1 or 2
	void response104(string respond, SOCKET _socket);
	void response106(vector<Room*> rooms, SOCKET _socket);
	string response108(vector<User*> users, bool error);
	void response110(int questionsNumber, int time, int status, Room* currRoom, SOCKET _socket); // 0 /1/2
	void response112(SOCKET _socket, bool succesOrNot);
	void response114(int status, SOCKET _socket); // 0 fail, 1 success
	void response116(SOCKET _socket);
	string response118(Question* question, User* user, Room* room);
	string response120(bool yesOrNot);
	string response121(vector<User*> users, DataBase DB);
	void response124(SOCKET _socket, vector<string> top3_Scores);
	void response126(SOCKET _socket, vector<string> personalStatus);


private:
	Helper _myHelper;
};
