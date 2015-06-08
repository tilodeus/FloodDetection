#include <iostream>
#include <stdio.h>
#include "FDModel.h"
#include <string>

using namespace std;

class FDView{

public:
	FDView(string message);
	~FDView();

	

private:
	void waitforInput();
	void generateMenu(string);
	void DoTheThing();
	void Unittest();
	void errorMessage();
	void exitProgram();

	FDModel model;
};