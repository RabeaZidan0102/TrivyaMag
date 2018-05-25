#include "Room.h"
#include <sstream>
#include <exception>

using std::stringstream;
using std::exception;

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

bool Room::joinRoom(User* user)
{
	if (_users.size() >= _maxUsers)
	{
		return false;
	}
	else
	{
		_users.push_back(user);
		sendMessage(user, getUserListMessage());
		return true;
	}
}

void Room::leaveRoom(User * user)
{
	for (unsigned int i = 0; i < _users.size(); i++)
	{
		if (user->getUsername() == _users[i]->getUsername())
		{
			_users.erase(_users.begin() + i -1);
			sendMessage(getUserListMessage());
		}
	}
}

int Room::closeRoom(User * user)
{
	if (user == _admin)
	{
		sendMessage(user, std::to_string(this->getID()));
		for (unsigned int i = 0; i < _users.size(); i++)
		{
			if (_users[i] != _admin)
			{
				_users[i]->clearRoom();
			}
		}
		return this->getID();
	}
	return -1;
}

vector<User*> Room::getUsers()
{
	return vector<User*>(_users);
}

string Room::getUserListMessage()
{
	stringstream msg108;
	msg108 << "108";

	if (_users.size() == 0)
	{
		msg108 << "#" << _users.size();
		for (unsigned int i = 0; i < _users.size(); i++)
		{
			msg108 << "#"<< _help.getPaddedNumber(_users[i]->getUsername().length(),2) <<"#" << _users[i]->getUsername();
		}
	}
	else
	{
		msg108 << "0";
	}

	return string(msg108.str());
}

int Room::getQuestionNumber()
{
	return _questionsNumber;
}

int Room::getID()
{
	return _id;
}

string Room::getAdminName()
{
	return string(_admin->getUsername());
}

void Room::sendMessage(string message)
{
	sendMessage(nullptr, message);
}

void Room::sendMessage(User * excludeUser, string message)
{
	try
	{
		if (excludeUser != nullptr)
		{
			for (unsigned int i = 0; i < _users.size(); i++)
			{
				if (_users[i]->getUsername() != excludeUser->getUsername())
				{
					_users[i]->send(message);
				}
			}
		}
		else
		{
			for (unsigned int i = 0; i < _users.size(); i++)
			{
				_users[i]->send(message);	
			}
		}
	}
	catch (exception& e)
	{
		cout << e.what() << endl;
	}
}
