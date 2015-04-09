#include "FDView.h"
#include <iostream>
#include <stdio.h>

using namespace std;

FDView::FDView(){
	generateMenu();

}

FDView::~FDView(){

}

void FDView::generateMenu(){
	cout << "1) GENERATE\n2)exit\nInput: ";
}