#include "decoder.h"
#include <iostream>
#include <fstream>
#include <utility>

using namespace std;

pair<byte_t, byte_t> getNextTwoBytes(ifstream &file) {

  byte_t previous = file.get();
  byte_t current = file.get();

  return make_pair(previous, current);
}

int main(int argc, char* argv[]) {
  
  if (argc != 2) { // Note: only decodes one image
    cout << "Invalid Usage!" << "\n";
    cout << "Correct Usage: decoder.out <path-to-jpeg-file>";
    return 1;
  }

  // TODO: Maybe reject files not ending with .jpg?

  const string fileName = argv[1];
  
  ifstream infile;
  infile.open(fileName, ios::binary | ios::in);

  cout << "Decoding jpeg file: "<< fileName << "\n";

  auto [previous, current] = getNextTwoBytes(infile);

  if (previous != PREFIX || current != SOI) {
    cout << "Error - Not a JPEG file (does not begin with 0xFFD8)" << "\n";
    infile.close();
    return 1;
  } 

	return 0;	
}
