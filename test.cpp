#include <iostream>
#include "Validator.h"
#include "DataBase.h"
#include <algorithm>

int main()
{
	//Validator myTest;
	//std::cout << myTest.isPasswrodValid("s00Suray2") << std::endl;
	//std::cout << myTest.isUserNameValid("sdsdwe") << std::endl;

	DataBase* db = new DataBase();
	vector<string> v = db->getBestScores();
	string name = v[0];
	for (int current = 0; current < v.size(); current++)
	{
		cout << v[current] << endl;
	}

	vector<string> vectorName = db->getPersonalStatus("erez");
	for (unsigned int i = 0; i < vectorName.size(); i++)
	{
		cout << vectorName[i] << endl;
	}

	system("pause");
	return 0;
}
