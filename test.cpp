#include <iostream>
#include "Validator.h"
//#include "DataBase.h"
#include <algorithm>
#include "Room.h"
#include "User.h"
#include <WinSock2.h>
#include <Windows.h>
#include "Game.h"

int main()
{
	Validator myTest;
	std::cout << myTest.isPasswrodValid("s00Suray2") << std::endl;
	std::cout << myTest.isUserNameValid("sdsdwe") << std::endl;

	DataBase* db = new DataBase();
	vector<string> v = db->getBestScores();
	string name = v[0];
	for (unsigned int current = 0; current < v.size(); current++)
	{
		cout << v[current] << endl;
	}

	vector<string> vectorName = db->getPersonalStatus("erez");
	for (unsigned int i = 0; i < vectorName.size(); i++)
	{
		cout << vectorName[i] << endl;
	}
	//SOCKET _socket;
//	User* r = new User("saewe", _socket);
	//Room* room = new Room(4, r, "s", 5, 2,5);
	
	//room->getUserListMessage();

	//Game* gm;
	//gm->

	system("pause");
	return 0;
}
