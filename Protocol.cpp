#include "Protocol.h"
#include "User.h"
#include "Room.h"
#include "Question.h"
#include "DataBase.h"
#include <exception>

using std::exception;

Protocol::Protocol()
{
}

Protocol::~Protocol()
{
}

void Protocol::response102(string details, SOCKET _socket)
{
	stringstream res102;
	res102 << "102" << details;
	_myHelper.sendData(_socket, res102.str());
}

void Protocol::response104(string respond, SOCKET _socket)
{
	stringstream res104;
	res104 << "104" << respond;

	_myHelper.sendData(_socket, res104.str());
}

void Protocol::response106(vector<Room*> rooms, SOCKET _socket)
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

string Protocol::response108(vector<User*> users, bool error)
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

	return (res108.str());
}

/*
	status = 0 -> success
	status = 1 -> failed-room is full
	status = 2 -> failed-room not exist or other reason.
*/
void Protocol::response110(int questionsNumber, int time, int status, Room* currRoom, SOCKET _socket)
{
	stringstream res110; 
	
	if (status == 0)  // success
		res110 << "1100" << _myHelper.getPaddedNumber(questionsNumber, 2) << _myHelper.getPaddedNumber(time, 2);
	
	if (status == 1)
		res110 << "1101";

	if (status == 2)
		res110 << "1102";

	_myHelper.sendData(_socket, res110.str());

	if (status == 0)
	{
		_myHelper.sendData(_socket, currRoom->getUserListMessage());
	}
}

void Protocol::response112(SOCKET _socket, bool success)
{
	if (success)
	{
		_myHelper.sendData(_socket, "1120"); // success
	}
	else
	{
		response116(_socket);
	}
}

/*
	Status = 1 -> fail
	Status = 0 -> success
*/
void Protocol::response114(int status, SOCKET _socket)
{
	stringstream res114;
	res114 << 114 << status;
	_myHelper.sendData(_socket, res114.str());
}

void Protocol::response116(SOCKET _socket)
{
	_myHelper.sendData(_socket, "116");
}

string Protocol::response118(Question * question, User* user, Room* room)
{
	stringstream res118;

	if (question->getQuestion().length() == 0)
	{
		return "1180";
	}

	try
	{
		res118 << "118";
		res118 << _myHelper.getPaddedNumber(question->getQuestion().length(), 3) << question->getQuestion();

		for (unsigned int i = 0; i < question->getAnswers()->size(); i++)
		{
			res118 << _myHelper.getPaddedNumber(question->getAnswers()[i].length(), 3) << question->getAnswers()[i];
		}
	}
	catch (exception& e)
	{
		cout << e.what() << endl;
		if (user->getUsername() == room->getAdminName())
		{
			res118 << "1180";
		}
	}

	return (res118.str());
}

string Protocol::response120(bool yesOrNot)
{
	stringstream res120;
	res120 << "120" << yesOrNot;
	return (res120.str());
}

string Protocol::response121(vector<User*> users, DataBase DB)
{
	stringstream res121;
	res121 << "121" << _myHelper.getPaddedNumber(users.size(), 1);
	
	for (unsigned int i = 0; i < users.size(); i++)
	{
		res121 << _myHelper.getPaddedNumber(users[i]->getUsername().length(), 2) << users[i]->getUsername();
		res121 << _myHelper.getPaddedNumber(DB.getPersonalStatus(users[i]->getUsername())[1].length(), 2);
	}

	return (res121.str());
}

void Protocol::response124(SOCKET _socket, vector<string> top3_Scores)
{
	stringstream res124;

	res124 << "124";

	for (unsigned int i = 0; i < top3_Scores.size(); i++)
	{
		string name = top3_Scores[i].replace(top3_Scores[i].end() - 7, top3_Scores[i].end() - 6, "");
		res124 << _myHelper.getPaddedNumber(name.size() - 6, 2) << name;
	}

	_myHelper.sendData(_socket, res124.str());
}

void Protocol::response126(SOCKET _socket, vector<string> personalStatus)
{
	stringstream res126;
	res126 << "126";
	
	if (personalStatus[0].length() > 0)
	{
		res126 << _myHelper.getPaddedNumber(personalStatus[0].length(), 4);
		res126 << _myHelper.getPaddedNumber(personalStatus[1].length(), 6);
		res126 << _myHelper.getPaddedNumber(personalStatus[2].length(), 6);

		res126 << _myHelper.getPaddedNumber((personalStatus[3]).substr(0, 1).length(), 2);
		res126 << _myHelper.getPaddedNumber((personalStatus[3]).substr(2, 2).length(), 2);

		_myHelper.sendData(_socket, res126.str());
	}
	else
	{
		res126 << "0000";
		_myHelper.sendData(_socket, res126.str());
	}
}





