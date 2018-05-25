#pragma once

#include "User.h"
#include <vector>

using std::vector;
using std::cout;
using std::endl;

class Room
{
public:
	Room(int id, User* admin, string name, int maxUsers, int questionsNumber, int questionTime);
	~Room();
	bool joinRoom(User* user);
	void leaveRoom(User* user);
	int closeRoom(User* user);
	vector<User*> getUsers();
	string getUserListMessage();
	int getQuestionNumber();
	int getID();
	string getName;
	string getAdminName();

private:
	//string getUsersAsString(vector<User*> usersVector, User* user);
	void sendMessage(string message);
	void sendMessage(User* excludeUser, string message);

	vector<User*> _users;
	User* _admin;
	int _maxUsers;
	int _questionTime;
	int _questionsNumber;
	string _name;
	int _id;
	Helper _help;
};