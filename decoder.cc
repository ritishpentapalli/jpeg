#include <iostream>
#include <fstream>
#include "readFile.h"

using namespace std;

int generateHuffmanCodes(int tableFreqs[]) {

    uint16_t currCode = 0;
    
    for (int i=0; i<16; i++) {

      int freqOfi = tableFreqs[i];
      
      for (int j=0; j<freqOfi; j++) {
          cout << hex << currCode << " " << dec << i + 1<< "\n";
          currCode += 1;
      }
      if (freqOfi) {
        currCode <<= 1;
      }
    }

    return 0;

}

int main(int argc, char* argv[]) {
  
    if (argc != 2) { // Note: only decodes one image
    cout << "Invalid Usage!" << "\n";
    cout << "Correct Usage: decoder.out <path-to-jpeg-file>";
    return 1;
    }
    // TODO: Maybe reject files not ending with .jpg?

    const string fileName = argv[1];

    fileData* data = readFile(fileName);

    if (data == nullptr) {
        cout << "Null pointer returned\n";
        return 1; 
    }

    cout << "\n\n";

    int x = generateHuffmanCodes(data->huffmanTableFreqs[0][0]);

    return 0;
}