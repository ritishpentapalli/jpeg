#include <iostream>
#include "types.h"
#include "decodeHuffman.h"

using namespace std;
//TODO: Clean this function up, better naming
int generateHuffmanCodes(int tableFreqs[], byte_t huffmanSymbols[], HuffmanLookupTable &lookupTable) {

    uint16_t currCode = 0;
    int currSymbol = 0;
    
    for (int i=0; i<16; i++) {
      int freqOfi = tableFreqs[i];

      for (int j=0; j<freqOfi; j++) {
          
          lookupTable.table[i+1][currCode] = huffmanSymbols[currSymbol];
          // cout << hex << currCode << " " << dec << i + 1 << " " << hex << (int) huffmanSymbols[currSymbol]<<"\n";
          currCode += 1;
          currSymbol += 1;
      }
      if (freqOfi) {
        currCode <<= 1;
      }
    }
    return 0;
}

