#pragma once

#include "Question.h"
#include <vector>
#include <unordered_map>
#include "sqlite3.h"

using std::vector;
using std::cout;
using std::endl;
using std::unordered_map;
using std::pair;

class DataBase
{
public:

	DataBase();
	~DataBase();
	bool isUserExist(string username);
	bool addNewUser(string username, string password, string email);
	bool isUserAndPassMatch(string username, string password);
	vector<Question*> initQuestion(int numberOfQustions);
	vector<string> getBestScores();
	vector<string> getPersonalStatus();
	int insertNewGame();
	bool updateGameStatus();
	bool addAnswerToPlayer(int gameID, string username, int questionID, string answer, bool isCorrect, int answerTime);

private:
	sqlite3 * _db;
	char* zErrMsg = 0;
	unordered_map<string, vector<string>> results;

	static int callbackCount(void*, int, char**, char**);
	static int callbackQuestions(void*, int, char**, char**);
	static int callbackBestScore(void*, int, char**, char**);
	static int callbackPersonalStatus(void*, int, char**, char**);
};