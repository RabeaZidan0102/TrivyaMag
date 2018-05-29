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
			_questions = _db.initQuestion(questionNumber);
			_players = players;

			for (playersItr = _players.begin(); playersItr != _players.end(); ++playersItr)
			{
				(*playersItr)->setGame(this);
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
			msg121 << "#" << (*playersItr)->getUsername() << " " << _db.getPlayersScore()[(*playersItr)->getUsername()];
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

	return false;
}

bool Game::leaveGame(User * currUser)
{
	return false;
}

int Game::getID()
{
	return _gameID;
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
