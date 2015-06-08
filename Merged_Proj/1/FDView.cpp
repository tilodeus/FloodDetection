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
	cout << "    1) Detect flooded areas on input file\n    2) Run unittests\n    3) Exit\n\n Input: ";
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
		case 2: Unittest();
			break;
		case 3: exitProgram();
			break;
		default: errorMessage();
			break;
		}
	}
}

void FDView::DoTheThing(){
	system("cls");
	cout << "Processing..." << endl;
	model.Analyze();
	for (int i = 0; i < model.goodnesses.size(); i++)
	{
		cout << i+1 << ".pass: has " << model.goodnesses[i] << "% accuracy finding flooded areas." << endl;
	}
	cout << "Press ENTER to continue" << endl;
	cin.get(); cin.get();
	generateMenu("Please choose from the menu items listed below!\n");
}

void FDView::Unittest(){
	if (model.runUnittests())
		cout << "Unittested parts working as expected. All works as it should be (maybe...)" << endl;
	else
		cout << "One or more unittests failed... but I'm not saying which one." << endl;
	cin.get(); cin.get();
	generateMenu("Please choose from the menu items listed below!\n");
}

void FDView::errorMessage(){
	generateMenu("Not acceptable input. Please choose from the menu items listed below!\n");
}

void FDView::exitProgram(){
	exit;
}