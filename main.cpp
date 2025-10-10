//main.cpp

#include <iostream>
#include "VaREngine.h"
#include "CMO.h"
#include <fstream>
#include <vector>
#include <string>
using namespace std;

//void runCMO();

int main() {

	cout << "welcome to the MHA's workspace.....press 1 to open VAR calculator and 2 to open a CMO simulator...: " << endl;
	int choice;
	cin >> choice;


	if (choice == 1)
	{
		cout << "Monte Carlo VaR Project" << endl;
		VaR();
	}
	if (choice == 2)
	{
		cout << "Collateralized Mortgage Obligation (CMO) Project" << endl;
		runCMO();
	}

	return 0;
}