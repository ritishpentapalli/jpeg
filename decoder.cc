#include "decoder.h"
#include <iostream>
#include <fstream>

using namespace std;

int main(int argc, char* argv[]) {
  
  if (argc != 2) {
    cout << "Invalid Usage!" << "\n";
    cout << "Correct Usage: decoder.out <path-to-jpeg-file>";
    return 1;
  }

  const string fileName = argv[1];
  
  ifstream infile;
  infile.open(fileName, ios::binary | ios::in);
  cout << "Decoding jpeg file: "<< fileName << "\n";

  byte_t previous = infile.get();
  byte_t current = infile.get();


  if (previous == PREFIX && current == SOI) {
    cout << "Works so far" << "\n";
  } 
	return 0;	

}
