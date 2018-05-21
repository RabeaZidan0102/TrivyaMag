#include "DataBase.h"
#include <exception>
#include <sstream>
#include <time.h>
#include <algorithm> 
#include <chrono>
#include <ctime>
#include <map>

#define RETRIVING_ERROR "ERROR, while retriving information."
#define INSERTING_ERROR "ERROR, while inserting information."

using std::exception;
using std::stringstream;
using std::chrono::system_clock;
using std::ctime;
using std::map;

unordered_map<string, vector<string>> results;

#pragma warning(disable : 4996)

DataBase::DataBase()
{
	int rc;
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
		int questionCounter = 0;
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
	return _questionsVector;
}

vector<string> DataBase::getBestScores()
{
	int rc;
	stringstream getIsCorrectQuestion;
	vector<string> _allOFPlayersWithIsCorrectAnswer;
	vector<string> _bestScoreUsernames;

	try
	{
		// we have a vector of 
		getIsCorrectQuestion << "SELECT username from t_players_answers WHERE is_correct = 1";
		rc = sqlite3_exec(_db, getIsCorrectQuestion.str().c_str(), callbackBestScore, 0, &zErrMsg);
		
		if (rc != SQLITE_OK)
		{
			throw exception(RETRIVING_ERROR);
		}
		else
		{
			for (unsigned int i = 0; i < results["username"].size(); i++)
			{
				_allOFPlayersWithIsCorrectAnswer.push_back(results["username"][i]);
			}

			// string username, number of repetead time of the names
			map<string, int> usernamesCommon;
			map<string, int>::iterator it;

			for (unsigned int i = 0; i < results["username"].size(); i++)
			{
				usernamesCommon.insert(pair<string, int>(_allOFPlayersWithIsCorrectAnswer[i], 0));
				for (it = usernamesCommon.begin(); it != usernamesCommon.end(); ++it)
				{
					if (it->first == _allOFPlayersWithIsCorrectAnswer[i])
					{
						int c = usernamesCommon[it->first];
						usernamesCommon[it->first] = c + 1;
					}
				}
				
			}
			vector<std::pair<std::string, int>> topThree(3);
			partial_sort_copy(usernamesCommon.begin(), usernamesCommon.end(), topThree.begin(), topThree.end(),
				[](std::pair<const std::string, int> const& l,
					std::pair<const std::string, int> const& r)
			{
				return l.second > r.second;
			});

			try
			{
				_bestScoreUsernames.push_back(topThree[0].first);
				_bestScoreUsernames.push_back(topThree[1].first);
				_bestScoreUsernames.push_back(topThree[2].first);
			}
			catch (exception& e)
			{
				cout << e.what() << endl;
			}
			
			return _bestScoreUsernames;
		}

	}
	catch (exception& e)
	{
		cout << e.what() << endl;
		return _bestScoreUsernames;
	}
	return _bestScoreUsernames;
}

vector<string> DataBase::getPersonalStatus()
{
	vector<string> personalStatus;
	stringstream getStatus;

	getStatus << ""



	return vector<string>(personalStatus);
}

int DataBase::insertNewGame()
{
	int rc;
	stringstream addingNewGame, gettingTheID;
	
	time_t timeNow = time(0);
	string date = ctime(&timeNow);

	addingNewGame << "insert into t_games values(" << 0 << ", " << date << ", NULL)";
	try
	{
		rc = sqlite3_exec(_db, addingNewGame.str().c_str(), NULL, 0, &zErrMsg);
		if (rc != SQLITE_OK)
		{
			throw exception(INSERTING_ERROR);
		}
		else
		{
			_currentGameID++;
			return _currentGameID;
		}

	}
	catch (exception &e)
	{
		cout << e.what() << endl;
	}
	return _currentGameID;
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
