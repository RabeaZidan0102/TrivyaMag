#pragma once

#include <iostream>
#include <string>

using std::string;

/*
	valid password: 4 chars min, we dont have space, min 1 number, 1 upper and lower letter.
	valid username: starting with char, we dont have spaces, !NULL
*/

class Validator
{
public:
	static bool isPasswrodValid(string password);
	static bool isUserNameValid(string username);
};
