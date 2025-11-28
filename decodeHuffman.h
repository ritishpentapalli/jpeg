#ifndef DECODEHUFFMAN_H
#define DECODEHUFFMAN_H

#include <cstdint>
#include <map>

using namespace std;

struct HuffmanLookupTable {
    int tableID;
    int tableClass;
    map<int, map<uint16_t, byte_t> > table; // { length: { code: symbol } }
};

int generateHuffmanCodes(int tableFreqs[], byte_t huffmanSymbols[], HuffmanLookupTable &lookupTable);

#endif