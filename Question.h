#pragma once

#include <iostream>
#include <string>

using std::string;

class Question
{
public:
	Question(int id, string question, string correctAnswer, string answer2, string answer3, string answer4);
	~Question();
	string getQuestion();
	string* getAnswers();
	int getCorrectAnswerIndex();
	int getID();

private:
	string _question;
	string _answers[4];
	int _correctAnswerIndex;
	int _id;
};
