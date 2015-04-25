#include "FDView.h"

FDView::FDView(string message){
	generateMenu(message);
}

FDView::~FDView(){

}

void FDView::generateMenu(string message){
	system("cls");
	if (message != "")
		cout << "\nProgram's message:\n\n " << message << endl;
	cout << "    1) Detect flooded areas on input file\n    2) Exit\n\n Input: ";
	waitforInput();
}

void FDView::waitforInput(){

	int a;
	cin >> a;
	if (!cin){ // if the input is not int
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
	model.Init();
	model.LoadImage();
	model.Process();
	generateMenu("Please choose from the menu items listed below!\n");
}

void FDView::errorMessage(){
	generateMenu("Not acceptable input. Please choose from the menu items listed below!\n");
}

void FDView::exitProgram(){
	exit;
}