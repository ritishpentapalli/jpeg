#include <iostream>
#include <fstream>
#include "readFile.h"
#include "decodeHuffman.h"

using namespace std;

/*
This function was entirely vibecoded, using just to debug.
*/
void printHuffmanTable(const HuffmanLookupTable &tbl) {
    cout << "Huffman Table (class=" << tbl.tableClass
         << ", id=" << tbl.tableID << ")\n";

    for (const auto &lenEntry : tbl.table) {
        int length = lenEntry.first;
        const auto &codes = lenEntry.second;

        cout << "Length " << length << " bits:\n";

        for (const auto &codeEntry : codes) {
            uint16_t code = codeEntry.first;
            byte_t symbol = codeEntry.second;

            cout << "  code = "
                 << hex << code
                 << "  symbol = "
                 << hex << (int)symbol
                 << dec << "\n";
        }
    }
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

    HuffmanLookupTable DC0;
    DC0.tableID = 0;
    DC0.tableClass = 0;

    HuffmanLookupTable DC1;
    DC0.tableID = 0;
    DC0.tableClass = 1;

    HuffmanLookupTable AC0;
    DC0.tableID = 1;
    DC0.tableClass = 0;

    HuffmanLookupTable AC1;
    DC0.tableID = 1;
    DC0.tableClass = 1;
   
    int x = generateHuffmanCodes(data->huffmanTableFreqs[0][0], data->huffmanTables[0][0], DC0);
    x = generateHuffmanCodes(data->huffmanTableFreqs[0][0], data->huffmanTables[0][0], DC1);
    x = generateHuffmanCodes(data->huffmanTableFreqs[0][0], data->huffmanTables[0][0], AC0);
    x = generateHuffmanCodes(data->huffmanTableFreqs[0][0], data->huffmanTables[0][0], AC1);

    return 0;
}