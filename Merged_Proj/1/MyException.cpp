#include "MyException.h"

string MyException::errorMessage()
{
	return message;
}

void MyException::setErrorMessage(string param)
{
	message = param;
}
