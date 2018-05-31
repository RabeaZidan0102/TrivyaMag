#include "TriviaServer.h"
#include "Game.h"
#include "RecievedMessage.h"
#include "Room.h"
#include "Question.h"
#include <exception>

using std::exception;

TriviaServer::TriviaServer()
{

	try
	{
		_socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		if (_socket == INVALID_SOCKET)
		{
			throw std::exception(__FUNCTION__ " - socket");
		}
	}
	catch (exception& e)
	{
		cout << e.what() << endl;
	}
}

TriviaServer::~TriviaServer()
{

}
