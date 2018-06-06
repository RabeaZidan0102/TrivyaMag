#include "Validator.h"

bool Validator::isPasswrodValid(string password)
{
	int lowerCounter = 0, upperCounter = 0, digitCounter = 0;
	bool isSpace = false;

	int num = password.length();

	if (password.length() >= 4)
	{
		for (unsigned int i = 0; i < password.length(); i++)
		{
			if (islower(password[i])) { lowerCounter++; }
			if (isupper(password[i])) { upperCounter++; }
			if (isdigit(password[i])) { digitCounter++; }
			
			if(isspace(password[i]))
			{
				isSpace = true;
			}
		}

		if (lowerCounter &&  upperCounter && digitCounter && !isSpace)
		{
			return true;
		}
	}

	return false;
}

bool Validator::isUserNameValid(string username)
{
	bool returnedValue = false;

	if (username.size() > 0 && isalpha(username[0]))
	{
		for (unsigned int i = 0; i < username.length(); i++)
		{
			if (!isspace(username[i]))
			{
				returnedValue = true;
			}
		}
	}

	return returnedValue;
}
