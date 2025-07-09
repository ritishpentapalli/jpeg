#include "decoder.h"
#include "decodeAPPn.h"
#include <iostream>
#include <fstream>

using namespace std;

int readStartOfFrame(ifstream &infile) {

    int SoFData[12] = {0};

    int length = (infile.get() << 8) + infile.get();
    length -= 2;

    int precision = infile.get();
    length -= 1;
    
    int height = (infile.get() << 8) + infile.get();
    int width = (infile.get() << 8) + infile.get();
    length -= 4;
    SoFData[0] = height;
    SoFData[1] = width;

    int numberOfChannels = infile.get();
    length -= 1;
    SoFData[2] = numberOfChannels;

    SoFData[3] = (int) infile.get(); // Component ID
    SoFData[4] = (int) infile.get(); // Sampling Factor
    SoFData[5] = (int) infile.get(); // Quantization Table ID
    length -= 3;

    if (numberOfChannels == 1) { // Grayscale
        
    }
    else if (numberOfChannels == 3) { // YCbCr or RGB
        SoFData[6] = (int) infile.get();
        SoFData[7] = (int) infile.get();
        SoFData[8] = (int) infile.get();
        SoFData[9] = (int) infile.get();
        SoFData[10] = (int) infile.get();
        SoFData[11] = (int) infile.get();
        length -= 6;
    }
    else if (numberOfChannels == 4) { // CMYK
        cout << "CMYK jpeg\n";
        return 1; 
    }

    // for (int i=0; i<12; i++) {
    //     cout << SoFData[i] << " "; 
    // }
}

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
    

    // Reading Quantization Tables
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
    
    // Read SoF
    
    previous = infile.get();
    current = infile.get();

    if (previous != PREFIX) {
        cout << "Error - Did not find prefix before SoF\n";
        infile.close();
        return 1;
    }

    if (current == SOF0) { // Baseline DCT
        readStartOfFrame(infile);
    }
    return 0;
}
