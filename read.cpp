
// read a file into memory
#include <iostream>     // std::cout
#include <fstream>      // std::ifstream

using namespace std;

int main () {

  unsigned char bytes[4];
  int sum = 0;
  FILE *fp=fopen("header.data","rb");
  while ( fread(bytes, 4, 1,fp) != 0) {
      sum += bytes[0] + (bytes[1]<<8) + (bytes[2]<<16) + (bytes[3]<<24);
  }
  cout << sum << endl;

  return 0;
}

