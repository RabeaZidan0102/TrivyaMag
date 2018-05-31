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

void Protocol::response118(Question * question, SOCKET _socket, User* user, Room* room)
{
	stringstream res118;

	try
	{
		res118 << "118";
		res118 << _myHelper.getPaddedNumber(question->getQuestion().length(), 3) << question->getQuestion();

		for (unsigned int i = 0; i < question->getAnswers()->size(); i++)
		{
			res118 << _myHelper.getPaddedNumber(question->getAnswers()[i].length(), 3) << question->getAnswers()[i];
		}

		_myHelper.sendData(_socket, res118.str());
	}
	catch (exception& e)
	{
		cout << e.what() << endl;
		if (user->getUsername() == room->getAdminName())
		{
			_myHelper.sendData(_socket, "1180");
		}
	}
}

void Protocol::response120(int yesOrNot, SOCKET _socket)
{
	_myHelper.sendData(_socket, ("120" + yesOrNot));
}

void Protocol::response124(vector<User*> users, SOCKET _socket, DataBase DB)
{
	vector<string> top3_Scores = DB.getBestScores();
	stringstream res124;

	res124 << "124";

	for (unsigned int i = 0; i < top3_Scores.size(); i++)
	{
		string name = top3_Scores[i].replace(top3_Scores[i].end() - 7, top3_Scores[i].end() - 6, "");
		res124 << _myHelper.getPaddedNumber(name.size() - 6, 2) << name;
	}

	_myHelper.sendData(_socket, res124.str());
}





