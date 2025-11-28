#include <iostream>
#include "decodeHuffman.h"


int generateHuffmanCodes(int table[], int tableFreqs[]) {

    uint16_t currCode = 0;
    
    for (int i=0; i<16; i++) {

      int freqOfi = tableFreqs[i];
      
      for (int j=0; j<freqOfi; j++) {
          std::cout << std::hex << currCode << " " << std::dec << i + 1 << "\n";
          currCode += 1;
      }

      currCode << 1;
    }

    return 0;

}

