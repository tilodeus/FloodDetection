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
	cout << "1) Detect flooded areas on input file\n2) exit\n   Input: ";
	waitforInput();
}

void FDView::waitforInput(){

	int a;
	cin >> a;
		switch (a) {
		case 1: DoTheThing();
			break;
		case 2: exitProgram();
			break;
		default: defaultCase();
			break;
		}
}

void FDView::DoTheThing(){
	model.LoadImage();
	model.preProcess();
	model.Process();
	//salalala
	string message = "This space is mine and under construction, go elsewhere!\n";
	generateMenu(message);
}

void FDView::defaultCase(){
	string message = "Use the numbers in front of the menu items!\n";
	generateMenu(message);
}

void FDView::exitProgram(){
	exit;
}