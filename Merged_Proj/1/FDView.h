#include <iostream>
#include <stdio.h>
#include "FDModel.h"
#include <string>

using namespace std;

class FDView{

public:
	FDView(string message);
	~FDView();

	void waitforInput();

private:
	void generateMenu(string);
	void DoTheThing();
	void defaultCase();
	void exitProgram();

	FDModel model;
};