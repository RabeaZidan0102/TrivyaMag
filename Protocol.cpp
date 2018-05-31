#include "Protocol.h"
#include "User.h"
#include "Room.h"
#include "Question.h"


Protocol::Protocol(SOCKET sock) : _socket(sock)
{
}

Protocol::~Protocol()
{
}

void Protocol::response102(string details)
{
	stringstream res102;
	res102 << "102" << details;
	_myHelper.sendData(_socket, res102.str());
}

void Protocol::response104(string respond)
{
	stringstream res104;
	res104 << "104" << respond;

	_myHelper.sendData(_socket, res104.str());
}

void Protocol::response106(vector<Room*> rooms)
{
	stringstream res106;
	res106 << "106" << _myHelper.getPaddedNumber(rooms.size(), 4);

	for (unsigned int i = 0; i < rooms.size(); i++)
	{
		res106 << _myHelper.getPaddedNumber(rooms[i]->getID(), 4);
		res106 << _myHelper.getPaddedNumber(rooms[i]->getName().length(), 2) << rooms[i]->getName();
	}

	_myHelper.sendData(_socket, res106.str());
}

void Protocol::response108(vector<User*> users, bool error)
{
	stringstream res108;
	
	if (!error)
	{
		res108 << "108" << _myHelper.getPaddedNumber(users.size(), 3);

		for (unsigned int i = 0; i < users.size(); i++)
		{
			res108 << _myHelper.getPaddedNumber(users[i]->getUsername().length(), 2) << users[i]->getUsername();
		}
	}
	else
	{
		res108 << "1080";
	}

	_myHelper.sendData(_socket, res108.str());
}

void Protocol::response110(int questionsNumber, int time)
{

}


