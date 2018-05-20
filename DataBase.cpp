#include "DataBase.h"
#include <exception>
#include <sstream>
#include <time.h>
#include <algorithm> 
#include <chrono>
#include <ctime>

#define RETRIVING_ERROR "ERROR, while retriving information"

using std::exception;
using std::stringstream;
using std::chrono::system_clock;

unordered_map<string, vector<string>> results;

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

			gamesTable << "create table t_games(id integer primary key autoincrement, status int, start_time datetime, end_time datetime)";
			sqlite3_exec(_db, gamesTable.str().c_str(), NULL, 0, &zErrMsg);

			questionsTable << "create table t_questions(id intger primary key autoincrement, correctAnswer string, answer2 string, answer3 string, answer4 string)";
			sqlite3_exec(_db, questionsTable.str().c_str(), NULL, 0, &zErrMsg);
			
			usersTable << "create table t_users(username string primary key, password string, email string)";
			sqlite3_exec(_db, usersTable.str().c_str(), NULL, 0, &zErrMsg);
			
			playersAnswersTable << "create table t_players_answers(game_id integer primary key, username string primary key, question_id integer primary key, player_answer string, is_correct int, answer_time int, foreign key(game_id) REFERENCES games(id), foreign key(user_name) REFERENCES users(username), foreign key(question_id) REFERENCES questions(id))";
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
	sqlite3_close(this->_db);
	this->_db = nullptr;
}

bool DataBase::isUserExist(string username)
{
	int rc;
	bool returnedValue = true;

	stringstream showUsers;
	showUsers << "Select uaername from t_users Where username = " << username;
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

	addingUser << "insert into t_users values(" << username << ", " << password << ", " << email << ")";
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

	getting << "select * from t_users WHERE username = " << username << " AND password = " << password;
	rc = sqlite3_exec(_db, getting.str().c_str(), NULL, 0, &zErrMsg);

	if (rc == SQLITE_OK)
	{
		returnedValue = true;
	}

	return returnedValue;
}

vector<Question*> DataBase::initQuestion(int numberOfQustions)
{
	vector<Question*> _questionsVector;
	srand(time(NULL));

	try
	{
		unsigned int questionCounter = 0;
		int rc;
		string question, correctAnswer, answer2, answer3, answer4;
		stringstream questions;

		questions << "Select * FROM t_questions";
		rc = sqlite3_exec(_db, questions.str().c_str(), callbackQuestions, 0, &zErrMsg);

		if (rc != SQLITE_OK)
		{
			throw exception(RETRIVING_ERROR);
		}
		else
		{
			for (questionCounter = 0; questionCounter < numberOfQustions; questionCounter++)
			{
				question = results["question"][questionCounter];
				correctAnswer = results["correct_ans"][questionCounter];
				answer2 = results["ans2"][questionCounter];
				answer3 = results["ans3"][questionCounter];
				answer4 = results["ans4"][questionCounter];

				Question* _question = new Question(questionCounter + 1, question, correctAnswer, answer2, answer3, answer4);
				_questionsVector.push_back(_question);
			}
		}
	}
	catch (exception& e)
	{
		cout << e.what() << endl;
		return _questionsVector;
	}
}

vector<string> DataBase::getBestScores()
{
	int rc;
	stringstream getIsCorrectQuestion;
	vector<string> _bestScoreUsernames;

	try
	{
		getIsCorrectQuestion << "SELECT username, COUNT(*) from t_players_answers WHERE is_correct = 1 LIMIT 3";
		rc = sqlite3_exec(_db, getIsCorrectQuestion.str().c_str(), callbackBestScore, 0, &zErrMsg);
		
		if (rc != SQLITE_OK)
		{
			throw exception(RETRIVING_ERROR);
		}
		else
		{
			int i = 0;

			//while (i < results[].size())
			//{
				//_bestScoreUsernames
			//}

			return _bestScoreUsernames;
		}

	}
	catch (exception& e)
	{
		cout << e.what() << endl;
		return _bestScoreUsernames;
	}
}


int DataBase::insertNewGame()
{
	int rc;
	stringstream addingNewGame, gettingTheID;
	int returnedValue = NULL;
	
	system_clock::time_point p = system_clock::now();

	std::time_t t = system_clock::to_time_t(p);

	addingNewGame << "insert into games values( 1, " << std::ctime(&t) << ")";
	try
	{
		rc = sqlite3_exec(_db, addingNewGame.str().c_str(), NULL, 0, &zErrMsg);
		if (rc != SQLITE_OK)
		{
			throw("we have a problem!");
		}
	}
	catch (exception &e)
	{
		cout << e.what() << endl;

	}

	gettingTheID << "select id from t_games WHERE start_time = " << std::ctime(&t) << " AND status = 1";

	try
	{
		rc = sqlite3_exec(_db, gettingTheID.str().c_str(), callbackPersonalStatus, 0, &zErrMsg);
		if (rc != SQLITE_OK)
		{
			throw(RETRIVING_ERROR);
		}
		else
		{
			
		}
	}
	catch (exception &e)
	{
		cout << e.what() << endl;
	}
	

	return returnedValue;
}



int DataBase::callbackQuestions(void* notUsed, int argc, char** argv, char** azCol)
{
	int i;

	for (i = 0; i < argc; i++)
	{
		auto it = results.find(azCol[i]);
		if (it != results.end())
		{
			it->second.push_back(argv[i]);
		}
		else
		{
			pair<string, vector<string>> p;
			p.first = azCol[i];
			p.second.push_back(argv[i]);
			results.insert(p);
		}
	}

	return 0;
}

int DataBase::callbackBestScore(void* notUsed, int argc, char** argv, char** azCol)
{
	int i;

	for (i = 0; i < argc; i++)
	{
		auto it = results.find(azCol[i]);
		if (it != results.end())
		{
			it->second.push_back(argv[i]);
		}
		else
		{
			pair<string, vector<string>> p;
			p.first = azCol[i];
			p.second.push_back(argv[i]);
			results.insert(p);
		}
	}

	return 0;
}

int DataBase::callbackPersonalStatus(void* notUsed, int argc, char** argv, char** azCol)
{
	int i;

	for (i = 0; i < argc; i++)
	{
		auto it = results.find(azCol[i]);
		if (it != results.end())
		{
			it->second.push_back(argv[i]);
		}
		else
		{
			pair<string, vector<string>> p;
			p.first = azCol[i];
			p.second.push_back(argv[i]);
			results.insert(p);
		}
	}

	return 0;
}
