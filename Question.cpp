#include "Question.h"

Question::Question(int id, string question, string correctAnswer, string answer2, string answer3, string answer4)
{
	 int correctAnswerIndex, index2 = -1, index3 = -1, index4 = -1;
	_question = question;
	_id = id;

	correctAnswerIndex = rand() % 4;
	_correctAnswerIndex = correctAnswerIndex;
	_answers[correctAnswerIndex] = correctAnswer;

	while (index2 == correctAnswerIndex || index2 == -1)
	{
		index2 = rand() % 4;
	}
	_answers[index2] = answer2;

	while (index3 == correctAnswerIndex || index3 == index2 || index3 == -1)
	{
		index3 = rand() % 4;
	}
	_answers[index3] = answer3;

	while (index4 == correctAnswerIndex || index4 == index3 || index4 == index2 || index4 == -1)
	{
		index4 = rand() % 4;
	}
	_answers[index4] = answer4;

}

Question::~Question()
{
}

string Question::getQuestion()
{
	return string(_question);
}

string * Question::getAnswers()
{
	return new string[4]{ _answers[0], _answers[1], _answers[2], _answers[3] };
}

int Question::getCorrectAnswerIndex()
{
	return _correctAnswerIndex;
}

int Question::getID()
{
	return _id;
}
