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
        else if (tablePrecision == 1) { // 16 bit precision
            for (int i=0; i<64; i++) {
                qt[tableId][i] = (infile.get() << 8) + infile.get(); // since its 16 bit
                length -= 2;
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
        cout << "Decoder doesn't handle CMYK jpegs\n";
        return 1; 
    }

    if (length != 0) {
        cout << "SOF length != 0 after reading everything\n";
        return 1;
    }

    return 0;
}

int readHuffmanTable(ifstream &infile, byte_t huffmanTables[2][4][162], int huffmanTableFreqs[2][4][16], int huffmanTableFreqCount[2][4]) {

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
            huffmanTableFreqs[ACorDC][tableID][j] = number;
            huffmanTableFreqCount[ACorDC][tableID] += number;
        }

        for (int j=0;j<huffmanTableFreqCount[ACorDC][tableID];j++) {
            huffmanTables[ACorDC][tableID][j] = infile.get();
            length -= 1;
        }
    }

    if (length != 0) {
        return 1;
    }
    return 0;
}

fileData* readFile(const string &fileName) {

    ifstream infile;
    infile.open(fileName, ios::binary | ios::in);

    #ifdef DEBUGINFO
    cout << "Decoding jpeg file: "<< fileName << "\n\n";
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
            readAPPnSegment(infile, current); // Skipping all APPn segments
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

    #ifdef DEBUGINFO // print out QTables
    cout << "DQT Data \n\n";
    for (int i=0; i<4; i++) {
        cout << "Quantization Table ID:" << i;
        for (int j=0; j<64; j++) {
            if (j % 8 == 0) {
                cout << "\n";
            }
            cout << data->quantizationTables[i][j] << " ";
        }
        cout << "\n\n";
    }
    cout << "###############" << "\n\n";
    #endif

    // Read SoF
    previous = infile.get();
    current = infile.get();

    if (previous != PREFIX) {
        cout << "Error - Did not find 0xFF after QTables\n";
        infile.close();
        delete data;
        return nullptr;
    }

    if (current == SOF0) { // Baseline DCT
        int SOF_return = readStartOfFrame(infile, data->SoFData);
        if (SOF_return != 0) {
            cout << "Error reading SOF\n";
            infile.close();
            delete data;
            return nullptr;
        }
    }
    else if (current >= SOF1 && current <= SOF3) {
        cout << "Decoder only handles Baseline DCT\n";
        infile.close();
        delete data;
        return nullptr;
    }
    else {
        cout << "Error - Did not find a DCT marker\n";
        infile.close();
        delete data;
        return nullptr;
    }

    #ifdef DEBUGINFO // Print out SoF data
        cout << "SoF Data\n\n";
        cout << "Height: " << data->SoFData[0] << "\n";
        cout << "Width: " << data->SoFData[1] << "\n";
        cout << "Number of Channels: " << data->SoFData[2] << " -> ";
        if (data->SoFData[2] == 1) {
            cout << "Greyscale image\n\n";
            cout << "Component ID: " << data->SoFData[3] << "\n";
            cout << "Horizontal Sampling Factor: " << ((data->SoFData[4] & 0xF0) >> 4) << "\n";
            cout << "Vertical Sampling Factor: " << (data->SoFData[4] & 0x0F) << "\n";
            cout << "Quantization Table ID: " << data->SoFData[5] << "\n\n";
        }
        else if (data->SoFData[2] == 3) {
            cout << "YCBCr image\n\n";
            cout << "Y Component\n";
            cout << "Component ID: " << data->SoFData[3] << "\n";
            cout << "Horizontal Sampling Factor: " << ((data->SoFData[4] & 0xF0) >> 4) << "\n";
            cout << "Vertical Sampling Factor: " << (data->SoFData[4] & 0x0F) << "\n";
            cout << "Quantization Table ID: " << data->SoFData[5] << "\n\n";
            cout << "Cb Component\n";
            cout << "Component ID: " << data->SoFData[6] << "\n";
            cout << "Horizontal Sampling Factor: " << ((data->SoFData[7] & 0xF0) >> 4) << "\n";
            cout << "Vertical Sampling Factor: " << (data->SoFData[7] & 0x0F) << "\n";
            cout << "Quantization Table ID: " << data->SoFData[8] << "\n\n";
            cout << "Cr Component\n";
            cout << "Component ID: " << data->SoFData[9] << "\n";
            cout << "Horizontal Sampling Factor: " << ((data->SoFData[10] & 0xF0) >> 4) << "\n";
            cout << "Vertical Sampling Factor: " << (data->SoFData[10] & 0x0F) << "\n";
            cout << "Quantization Table ID: " << data->SoFData[11] << "\n\n";
            cout << "###############" << "\n\n";
        }
    #endif

    // TODO: Implement DRI
    previous = infile.get();
    current = infile.get();

    if (previous != PREFIX || current != DHT) {
        cout << "Error - Did not find DHT marker\n";
        infile.close();
        delete data;
        return nullptr;
    }

    int DHTstatus = readHuffmanTable(infile, data->huffmanTables, data->huffmanTableFreqs, data->huffmanTableFreqCount);
    if (DHTstatus != 0) {
        cout << "Error reading Huffman tables\n";
        infile.close();
        delete data;
        return nullptr;
    }
    
    #ifdef DEBUGINFO // Print out DHT data
        cout << "DHT Data\n\n";
        cout << "DC tables\n";
        for (int i=0; i<4; i++) {
            cout << "Table ID: " << i << "\n";
            for (int j=0; j<16; j++) {
                cout << dec << j+1 << ":" << data->huffmanTableFreqs[0][i][j] << "\n";
            }
            for (int k=0; k<data->huffmanTableFreqCount[0][i];k++) {
                cout << hex << (int) data->huffmanTables[0][i][k] << " ";
            }
            cout << "\n";
        }
         cout << "AC tables\n";
        for (int i=0; i<4; i++) {
            cout << "Table ID: " << i << "\n";
            for (int j=0; j<16; j++) {
                cout << dec << j+1 << ":" << data->huffmanTableFreqs[1][i][j] << "\n";
            }
            for (int k=0; k<data->huffmanTableFreqCount[1][i];k++) {
                cout << hex << (int) data->huffmanTables[1][i][k] << " ";
            }
            cout << "\n";
        }
    #endif

    previous = infile.get();
    current = infile.get();

    cout << hex << (uint) previous << hex << (uint) current;

    return data;

}