#include "Room.h"

Room::Room(int id, User * admin, string name, int maxUsers, int questionsNumber, int questionTime)
{
	_id = id;
	_name = name;
	_maxUsers = maxUsers;
	_questionsNumber = questionsNumber;
	_questionTime = questionTime;
	_admin = admin;

	_users.push_back(admin);
}

Room::~Room()
{
}

bool Room::joinRoom(User * user)
{
	return false;
}

string Room::getUserListMessage()
{

	return string();
}
