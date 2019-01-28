#include <iostream>

using namespace std;

int main(int argc, char const *argv[])
{

	//This is an example/test file for how to read in program arguments



	//Hello to the first passed argument
	cout << "Welcome " << argv[1] << endl;
		
	// ===================================
	// How to loop through all the arguments

	for (int i = 0; i < argc; ++i) 
        cout << argv[i] << "\n"; 

    //Read in a character to keep the command window open (only necessary if clicking on built exe file)
	cout << "Press any button to exit the program" << endl;
	char c;
	cin >> c;
}
