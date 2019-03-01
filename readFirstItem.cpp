#include <iostream>
#include <fstream>

using namespace std;

int main() {

    char preheader[72];
    char name[48];

    std::ifstream input("/VDITestFiles/Good/Test-fixed-1k.vdi");

    for (int i = 0; i < sizeof(preheader); i++) {
        input >> preheader[i];
        cout<< preheader[i] << endl;
    }
	for (int i = 0; i < sizeof(name); i++) {
        input >> name[i];
        cout<< name[i]<< endl;
    }
}