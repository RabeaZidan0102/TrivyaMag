#include "User.h"
#include "Game.h"
#include "Room.h"

User::User(string username, SOCKET socket) : _username(username), _sock(socket)
{
	_currGame = nullptr;
	_currRoom = nullptr;
}

User::~User()
{
}

void User::send(string message)
{
	_Helper.sendData(_sock, message);
}

string User::getUsername()
{
	return string(_username);
}

SOCKET User::getSocket()
{
	return SOCKET(_sock);
}

Room * User::getRoom()
{
	return _currRoom;
}

Game * User::getGame()
{
	return _currGame;
}

void User::setGame(Game * gm)
{
	_currRoom = nullptr;
	if (gm != nullptr)
	{
		_currGame = new Game(*gm);
	}
	else
	{
		_currGame = nullptr;
	}
}

void User::clearRoom()
{
	_currRoom = nullptr;
}

bool User::createRoom(int roomID, string roomName, int maxUsers, int questionNumber, int questionTime)
{
	if (_currRoom != nullptr)
	{
		return false;
	}
	
	_currRoom = new Room(roomID, this, roomName, maxUsers, questionNumber, questionTime);

	return true;
}

bool User::joinRoom(Room * newRoom)
{
	if (_currGame != nullptr)
	{
		return false;
	}
	if (newRoom->joinRoom(this))
	{
		_currRoom = newRoom;
	}

	return true;
}

void User::leaveRoom()
{
	if (this->_currRoom != nullptr)
	{
		_currRoom->leaveRoom(this);
		clearRoom();
	}

}

int User::closeRoom()
{
	int roomID;
	if (this->_currRoom == nullptr)
	{
		return -1;
	}
	else if(_currRoom->getAdminName() == this->getUsername())
	{
		roomID = _currRoom->getID();
		_currRoom->closeRoom(this);
		this->clearRoom();

		return roomID;
	}
	else
	{
		return -1;
	}

}

bool User::leaveGame()
{
	if (this->_currGame != nullptr)
	{
		if (_currGame->leaveGame(this))
		{
			return true;
		}
	}
	return false;
}




