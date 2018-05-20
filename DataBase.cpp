#include "DataBase.h"
#include <exception>
#include <sstream>

using std::exception;
using std::stringstream;

DataBase::DataBase()
{
	int rc;
	bool ans;
	rc = sqlite3_open("trivia.db", &_db);

	try
	{
		if (rc != SQLITE_OK)
		{
			throw (exception("Can't open database"));
			sqlite3_close(_db);
		}

		else
		{
			int rc;
			stringstream gamesTable, playersAnswersTable, questionsTable, usersTable;

			gamesTable << "create table games(id integer primary key autoincrement, status int, start_time datetime, end_time datetime)";
			sqlite3_exec(_db, gamesTable.str().c_str(), NULL, 0, &zErrMsg);

			questionsTable << "create table questions(id intger primary key autoincrement, correctAnswer string, answer2 string, answer3 string, answer4 string)";
			sqlite3_exec(_db, questionsTable.str().c_str(), NULL, 0, &zErrMsg);
			
			usersTable << "create table users(username string primary key, password string, email string)";
			sqlite3_exec(_db, usersTable.str().c_str(), NULL, 0, &zErrMsg);
			
			playersAnswersTable << "create table players_answers(game_id integer primary key, user_name string primary key, question_id integer primary key, player_answer string, is_correct int, answer_time int, foreign key(game_id) REFERENCES games(id), foreign key(user_name) REFERENCES users(username), foreign key(question_id) REFERENCES questions(id))";
			sqlite3_exec(_db, playersAnswersTable.str().c_str(), NULL, 0, &zErrMsg);
		}

	}

	catch (exception& e)
	{
		cout << e.what() << endl;
	}
}

DataBase::~DataBase()
{
}

bool DataBase::isUserExist(string username)
{
	int rc;
	bool returnedValue = true;

	stringstream showUsers;
	showUsers << "Select uaername from users Where username = " << username;
	rc = sqlite3_exec(_db, showUsers.str().c_str(), NULL, 0, &zErrMsg);
	
	if (rc != SQLITE_OK)
	{
		returnedValue = false;
	}

	return returnedValue;
}

bool DataBase::addNewUser(string username, string password, string email)
{
	int rc;
	bool returnedValue = true;
	stringstream addingUser;

	addingUser << "insert into users values(" << username << ", " << password << ", " << email << ")";
	rc = sqlite3_exec(_db, addingUser.str().c_str(), NULL, 0, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		returnedValue = false;
	}

	return returnedValue;
}

bool DataBase::isUserAndPassMatch(string username, string password)
{
	int rc;
	bool returnedValue = false;
	stringstream getting;

	getting << "select * from users WHERE username = " << username << " AND password = " << password;
	rc = sqlite3_exec(_db, getting.str().c_str(), NULL, 0, &zErrMsg);

	if (rc == SQLITE_OK)
	{
		returnedValue = true;
	}

	return returnedValue;
}

