#include "decoder.h"
#include "decodeAPPn.h"
#include <iostream>
#include <fstream>

using namespace std;

void readAPPnSegment(ifstream &infile, byte_t current) {
    
    uint length = (infile.get() << 8) + infile.get();
    for (int i=0; i<length-2; i++) { // length-2 because we already read two bytes when reading APPn marker
        infile.get();
    }
    // if (current == APP1) {
    //     cout << "APP1 found\n";
    //     decodeEXIF(infile);
    // }
}

int readQuantizationTable(ifstream &infile, int qt[4][64], int qtSet[4]) {

    int length = (infile.get() << 8) + infile.get(); // Big-endian
    length -= 2; // Since these two bytes are also included in the length

    
    while (length > 0) { // Keep looking for QTs until length is 0
        byte_t tableInfo = infile.get();
        length -= 1;

        int tablePrecision = tableInfo >> 4; // Left half of byte denotes precision
        int tableId = tableInfo & 0x0F; // Right half of byte denotes table ID

        if (tablePrecision != 0 && tablePrecision != 1) {
            cout << "Error - Table precision neither 0 nor 1\n";
            return 1;
        }
        if (tableId > 3) {
            cout << "Error - Table ID is greater than 3\n";
            return 1;
        }
        
        qtSet[tableId] = 1;
        if (tablePrecision == 0) { // 8 bit precision
            for (int i=0; i<64; i++) {
                qt[tableId][i] = infile.get();
                length -= 1;
            }
        }
        else if (tablePrecision == 1) {
            for (int i=0; i<64; i++) {
                qt[tableId][i] = (infile.get() << 8) + infile.get();
                length -= 1;
            } 
        }
    }

    if (length != 0) {
        cout << "Quantization segment length is not 1 after reading\n";
        return 1;
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

    ifstream infile;
    infile.open(fileName, ios::binary | ios::in);

    cout << "Decoding jpeg file: "<< fileName << "\n";

    // Read first two bytes
    byte_t previous = infile.get();
    byte_t current = infile.get();  

    // Check if they are SOI marker
    if (previous != PREFIX || current != SOI) {
        cout << "Error - Not a JPEG file (does not begin with 0xFFD8)\n";
        infile.close();
        return 1;
    }

    // Check and skip through all application segments
    bool readAllAPPn = false;
    while (!readAllAPPn) {
        previous = infile.get();
        current = infile.get();
        if (previous != PREFIX) {
            cout << "Error - Not a JPEG file\n"; // This really should not happen
        }

        readAllAPPn = true;
        if (current >= APP0 && current <= APP15) {
            readAPPnSegment(infile, current);
            readAllAPPn = false;
        }

    }

    // Confirm that next marker is DQT
    if (previous != PREFIX || current != DQT) {
        cout << "Error - Did not find DQT marker\n";
        infile.close();
        return 1;
    }
    
    int quantizationTables[4][64] = {0}; // JPEG standard allows for upto 4 QTs
    int quantizationTablesSet[4] = {0}; // Set to 1 if that QT exists
    // As an example, if only two QTs exist in the file, the array would be {1, 1, 0, 0}
    int quantizationStatus = readQuantizationTable(infile, quantizationTables, quantizationTablesSet);

    if (quantizationStatus != 0) {
        cout << "Error while reading Quantization Tables\n";
        infile.close();
        return 1;
    }

    // Inverse zigzag to put the QT values in the right position
    for (int j=0; j<4; j++) {
        int copyOfQT[64];
        for (int i = 0; i < 64; ++i) {
            copyOfQT[i] = quantizationTables[j][i]; // Copy is important because we're modifying values
        }
        for (int i=0; i<64; i++) {
            quantizationTables[j][zigZagMap[i]] = copyOfQT[i];
        }
    }
    
    // for (int j=0; j<4; j++) {
    //     for (int i=0; i < 64; i++) {
    //         if (i%8 == 0) {
    //             cout << "\n";
    //         }
    //         cout << quantizationTables[j][i] << " ";
    //     }
    //     cout << "\n";
    // }
    // for (int i=0; i<4; i++) {
    //     cout << quantizationTablesSet[i] << " ";
    // }
    









    return 0;
}
