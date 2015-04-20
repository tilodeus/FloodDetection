#include "FDView.h"

FDView::FDView(string message){
	generateMenu(message);
}

FDView::~FDView(){

}

void FDView::generateMenu(string message){
	system("cls");
	if (message != "")
		cout << "Program's message:\n" << message << endl;
	cout << " 1) Detect flooded areas on input file\n 2) Exit\n\n  Input: ";
	waitforInput();
}

void FDView::waitforInput(){

	int a;
	cin >> a;
	if (!cin){
		cin.clear();
		cin.ignore(256, '\n');
		errorMessage();
	}
	else{
		switch (a) {
		case 1: DoTheThing();
			break;
		case 2: exitProgram();
			break;
		default: errorMessage();
			break;
		}
	}
}

void FDView::DoTheThing(){
	model.LoadImage();
	model.Process();
	string message = "Please choose from the menu items listed below!\n";
	generateMenu(message);
}

void FDView::errorMessage(){
	string message = "Not acceptable input. Please choose from the menu items listed below!\n";
	generateMenu(message);
}

void FDView::exitProgram(){
	exit;
}