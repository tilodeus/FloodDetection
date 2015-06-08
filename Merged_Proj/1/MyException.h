#ifndef MY_EXCEPTION_H
#define MY_EXCEPTION_H


#include <exception>
#include <string>

using namespace std;

class MyException: public exception
{
private:
	string message;

public:
	string errorMessage();
	void setErrorMessage(string param);

};
#endif