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
	
	stringstream msg121;

	try
	{
		msg121 << "121# " << _players.size();
		for (playersItr = _players.begin(); playersItr != _players.end(); ++playersItr)
		{
			msg121 << "#" << _myHelper.getPaddedNumber((*playersItr)->getUsername().length(), 2);
			msg121 << "#" << (*playersItr)->getUsername() << _results[(*playersItr)->getUsername()];
		}

		for (playersItr = _players.begin(); playersItr != _players.end(); ++playersItr)
		{
			(*playersItr)->send(msg121.str());
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
			return false;
		}
	}
	else
	{
		_currQuestionIndex++;
		this->sendQuestionToAllUsers();
		return true;
	}
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
	
	user->send("120" + isCorrect);

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
	stringstream  msg118;
	string* answers = _questions[_currQuestionIndex]->getAnswers();

	msg118 << "118";


	for (playersItr = _players.begin(); playersItr != _players.end(); ++playersItr)
	{
		try
		{
			if (_questions[_currQuestionIndex]->getQuestion().length() > 0)
			{
				msg118 << "#" << _myHelper.getPaddedNumber(_questions[_currQuestionIndex]->getQuestion().length(), 3) << "#" << _questions[_currQuestionIndex]->getQuestion();
				for (unsigned int i = 0; i < answers->size(); i++)
				{
					msg118 << "#" << _myHelper.getPaddedNumber(answers[i].length(), 3) << "#" << answers[i];
				}		
					
				(*playersItr)->send(msg118.str());
				
			}
			else
			{
				msg118 << "0";

				if ((*playersItr)->getRoom()->getAdminName() == (*playersItr)->getUsername())
				{
					(*playersItr)->send(msg118.str());
				}
			}
		}
		catch (exception& e)
		{
			cout << e.what() << endl;
		}
	}
}
