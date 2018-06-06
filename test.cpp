#include <iostream>
#include "Validator.h"
//#include "DataBase.h"
#include <algorithm>
#include "Room.h"
#include "User.h"
#include "TriviaServer.h"
#include "Game.h"
#include "TriviaServer.h"
#include "WSAInitializer.h"
#include <exception>

using std::exception;

int main()
{	
	/*
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

	string names = v[0].replace(v[0].end()-7, v[0].end()-6, "");
	cout << names << endl;

	cout << "\n\n\n\n";
	vector<string> _vectorStatus = db->getPersonalStatus("erez");
	for (unsigned int i = 0; i < _vectorStatus.size(); i++)
	{
		cout << _vectorStatus[i] << endl;
	}


	vector<string> vectorName = db->getPersonalStatus("erez");
	for (unsigned int i = 0; i < vectorName.size(); i++)
	{
		cout << vectorName[i] << endl;
	}

	TriviaServer* myServer = new TriviaServer();
	myServer->serve();
     //                       5      5+x+2    5+x+2+y+2                  
	string msgInString = "20005suray04abcd10suray@wert";

	cout << msgInString.substr(5, std::stoi(msgInString.substr(3, 2))) << endl;
	cout << msgInString.substr(5 + std::stoi(msgInString.substr(3, 2)) + 2, std::stoi(msgInString.substr(std::stoi(msgInString.substr(3, 2)) + 5, 2))) << endl;
	
	int x = std::stoi(msgInString.substr(3, 2));
	int y = std::stoi(msgInString.substr(std::stoi(msgInString.substr(3, 2)) + 5, 2));
	cout << msgInString.substr(5 + x + 2 + y + 2, std::stoi(msgInString.substr(5 + x + 2 + y, 2))) << endl;
	*/

	try
	{
		WSAInitializer wsaInit;
		TriviaServer* myServer = new TriviaServer();

		myServer->serve();
	}
	catch (exception& e)
	{
		cout << "Error occured: " << e.what() << endl;
	}
	system("PAUSE");
	return 0;
}
