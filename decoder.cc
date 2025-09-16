#include <iostream>
#include <fstream>
#include "readFile.h"

using namespace std;

int main(int argc, char* argv[]) {
  
    if (argc != 2) { // Note: only decodes one image
    cout << "Invalid Usage!" << "\n";
    cout << "Correct Usage: decoder.out <path-to-jpeg-file>";
    return 1;
    }
    // TODO: Maybe reject files not ending with .jpg?

    const string fileName = argv[1];

    fileData* data = readFile(fileName);
    return 0;
}