#include "readFile.h"
#include <iostream>
#include <fstream>

using namespace std;

void readAPPnSegment(ifstream &infile, byte_t current) {

    // TODO: Write decoder for different APPn segments
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
        cout << "Quantization segment length is not 0 after reading\n";
        return 1;
    }

    return 0;   
}

int readStartOfFrame(ifstream &infile, int SoFData[]) {

    // TODO: isGreyscale

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

fileData* readFile(const string &fileName) {

    ifstream infile;
    infile.open(fileName, ios::binary | ios::in);

    #ifdef DEBUGINFO
    cout << "Decoding jpeg file: "<< fileName << "\n";
    #endif

    // Read first two bytes
    byte_t previous = infile.get();
    byte_t current = infile.get();  

    // Check if they are SOI marker
    if (previous != PREFIX || current != SOI) {
        cout << "Error - Not a JPEG file (does not begin with 0xFFD8)\n";
        infile.close();
        return nullptr;
    }

    fileData* data = new fileData;

    bool readAllAPPn = false;
    while (!readAllAPPn) {
        previous = infile.get();
        current = infile.get();
        if (previous != PREFIX) {
            cout << "Error - Not a JPEG file\n"; // Byte right after SOI marker should be FF
        }

        readAllAPPn = true;
        if (current >= APP0 && current <= APP15) { // Checking for any application segment
            readAPPnSegment(infile, current);
            readAllAPPn = false;
        }

    }

    // Confirm that next marker is DQT
    if (previous != PREFIX || current != DQT) {
        cout << "Error - Did not find DQT marker\n";
        infile.close();
        delete data;
        return nullptr;
    }

    int quantizationStatus = readQuantizationTable(infile, data->quantizationTables, data->quantizationTablesSet);

    if (quantizationStatus != 0) {
        cout << "Error while reading Quantization Tables\n";
        infile.close();
        delete data;
        return nullptr;
    }

    int zigZagMap[] = { // Zigzag map for quantization tables
    0, 1, 8, 16, 9, 2, 3, 10,
    17, 24, 32, 25, 18, 11, 4, 5,
    12, 19, 26, 33, 40, 48, 41, 34,
    27, 20, 13, 6, 7, 14, 21, 28,
    35, 42, 49, 56, 57, 50, 43, 36,
    29, 22, 15, 23, 30, 37, 44, 51,
    58, 59, 52, 45, 38, 31, 39, 46,
    53, 60, 61, 54, 47, 55, 62, 63
    };

    // Inverse zigzag to put the QT values in the right position
    for (int j=0; j<4; j++) {
        int copyOfQT[64];
        for (int i = 0; i < 64; ++i) {
            copyOfQT[i] = data->quantizationTables[j][i]; // Copy is important because we're modifying values
        }
        for (int i=0; i<64; i++) {
            data->quantizationTables[j][zigZagMap[i]] = copyOfQT[i];
        }
    }

    // Read SoF
    previous = infile.get();
    current = infile.get();

    if (previous != PREFIX) {
        cout << "Error - Did not find prefix before SoF\n";
        infile.close();
        delete data;
        return nullptr;
    }

    if (current == SOF0) { // Baseline DCT
        readStartOfFrame(infile, data->SoFData);
    }

    #ifdef DEBUGINFO
        cout << "SoF Data\n";
        cout << "Height: " << SoFData[0] << "\n";
        cout << "Width: " << SoFData[1] << "\n";
        cout << "Number of Channels: " << SoFData[2] << "\n\n";
    #endif

    // TODO: Implement DRI
    previous = infile.get();
    current = infile.get();

    cout << hex << (uint) previous << hex << (uint) current;

    if (previous != PREFIX || current != DHT) {
        cout << "Error - Did not find DHT marker\n";
        infile.close();
        delete data;
        return nullptr;
    }

    int length = (infile.get() << 8) + infile.get();
    length -= 2;

    while (length>0) {

        byte_t tableInfo = infile.get();
        length -= 1;
        byte_t ACorDC = tableInfo>>4;
        byte_t tableID = tableInfo&0x0F;

        for (int j=0; j<16;j++) {
            int number = (int) infile.get();
            length -= 1;
            data->huffmanTableFreqs[ACorDC][tableID][j] = number;
            data->huffmanTableFreqCount[ACorDC][tableID] += number;
        }

        for (int j=0;j<data->huffmanTableFreqCount[ACorDC][tableID];j++) {
            data->huffmanTables[ACorDC][tableID][j] = infile.get();
            length -= 1;
        }
        
    }

    previous = infile.get();
    current = infile.get();

    cout << hex << (uint) previous << hex << (uint) current;

    return data;

}