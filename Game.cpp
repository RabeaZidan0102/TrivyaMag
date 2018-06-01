#include <exception>
#include "Game.h"
#include "User.h"
#include "Question.h"
#include "Room.h"
#include <sstream>
#include <string>

using std::stringstream;
using std::exception;

Game::Game(const vector<User*> &players, int questionNumber, DataBase &db) : _db(db)
{
	try
	{
		_questionNumber = questionNumber;
		_gameID = _db.insertNewGame();
		_currentTurnAnswers = 0;

		if (_gameID == -1)
		{
			throw exception("ERROR, in inserting new game");
		}
		else
		{
			this->initQuestionFromDB();
			_players = players;

			for (playersItr = _players.begin(); playersItr != _players.end(); ++playersItr)
			{
				(*playersItr)->setGame(this);
				_results[(*playersItr)->getUsername()] = 0;
			}
		}
	}
	catch (exception& e)
	{
		cout << e.what() << endl;
	}
}

Game::Game(Game& other) : _db(other._db)
{
	_questions = other._questions;
	_players = other._players;
	_questionNumber = other._questionNumber;
	_currQuestionIndex = other._currQuestionIndex;
	_gameID = other._gameID;
	_currentTurnAnswers = other._currentTurnAnswers;
	_results = other._results;
}

Game::~Game()
{
	_players.clear();
	_results.clear();
}

void Game::sendFirstQuestion()
{
	this->sendQuestionToAllUsers();
}

void Game::handleFinishGame()
{
	_db.updateGameStatus(this->getID());
	
	try
	{
		for (playersItr = _players.begin(); playersItr != _players.end(); ++playersItr)
		{
			(*playersItr)->send(_Protocol.response121(_players, _db));
			(*playersItr)->setGame(nullptr);
		}
	}
	catch (exception& e)
	{
		cout << e.what() << endl;
	}
}

bool Game::handleNextTurn()
{
	bool ans = true;
	// there is no one in the game 
	if (_players.size() == 0)
	{
		this->handleFinishGame();
		return false;
	}
	if (_players.size() == _currentTurnAnswers)
	{
		if (_currQuestionIndex = _questionNumber + 1)
		{
			this->handleFinishGame();
		}
	}
	else
	{
		_currQuestionIndex++;
		this->sendQuestionToAllUsers();
	}
	return ans;
}

bool Game::handleAnswerFromUser(User * user, int answerNumber, int time)
{
	_currentTurnAnswers++;
	bool isCorrect = false;
	
	if (answerNumber == _questions[_currQuestionIndex]->getCorrectAnswerIndex() && answerNumber < 5)
	{
		_results[user->getUsername()] = _results[user->getUsername()] + 1;
		isCorrect = true;
	}

	if (answerNumber < 5)
	{
	_db.addAnswerToPlayer(_gameID, user->getUsername(), _questions[_currQuestionIndex]->getID(), _questions[_currQuestionIndex]->getAnswers()[answerNumber - 1], isCorrect, time);
	}
	else
	{
		_db.addAnswerToPlayer(_gameID, user->getUsername(), _questions[_currQuestionIndex]->getID(), "", isCorrect, time);
	}
	
	user->send(_Protocol.response120(isCorrect));

	return (this->handleNextTurn());
}

bool Game::leaveGame(User * currUser)
{	
	for (playersItr = _players.begin(); playersItr != _players.end(); ++playersItr)
	{
		if (*playersItr == currUser)
		{
			_players.erase(playersItr);
			break;
		}
	}

	return this->handleNextTurn();
}

int Game::getID()
{
	return _gameID;
}

void Game::initQuestionFromDB()
{
	_questions = _db.initQuestion(_questionNumber);
}

void Game::sendQuestionToAllUsers()
{
	// 118 message to the server
	_currentTurnAnswers = 0;

	for (playersItr = _players.begin(); playersItr != _players.end(); ++playersItr)
	{
		if (_questions[_currQuestionIndex]->getQuestion().length() > 0)
		{
			(*playersItr)->send(_Protocol.response118(_questions[_currQuestionIndex], (*playersItr), (*playersItr)->getRoom()));
		}
	}
}
