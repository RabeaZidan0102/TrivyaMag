#pragma once

#include <map>
#include "DataBase.h"
#include "Helper.h"
#include "Protocol.h"

using std::map;

class Question;
class User;
class Room;

class Game
{
public:
	Game(const vector<User*> &players, int questionNumber, DataBase &db);
	Game(Game& other);
	~Game();
	void sendFirstQuestion();
	void handleFinishGame();
	bool handleNextTurn();
	bool handleAnswerFromUser(User* user, int answerNumber, int time);
	bool leaveGame(User* currUser);
	int getID();


private:
	void initQuestionFromDB();
	void sendQuestionToAllUsers();

	vector<Question*> _questions;

	vector<User*> _players;
	vector<User*>::iterator playersItr;

	int _questionNumber;
	int _currQuestionIndex;
	int _gameID;

	DataBase& _db;
	
	//username, point(correct answers)
	map<string, int> _results;
	//numbers of players that returned their answers in the round
	int _currentTurnAnswers;
	Helper _myHelper;
	Protocol _Protocol;
};
