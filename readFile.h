#ifndef READFILE_H
#define READFILE_H
#include <vector>

typedef unsigned char byte_t;

// JPEG Markers

const byte_t PREFIX = 0xFF;

const byte_t SOI = 0xD8; // Start Of Image
const byte_t DQT = 0xDB; // Define Quantization Table
const byte_t SOF0 = 0xC0; // Baseline DCT
const byte_t DHT = 0xC4; // Define Huffman Table
const byte_t SOS = 0xDA; // Start Of Scan
const byte_t EOI = 0xD9; // End Of Image

const byte_t APP0 = 0xE0; // Application markers 0-15
const byte_t APP1 = 0xE1;
const byte_t APP2 = 0xE2;
const byte_t APP3 = 0xE3;
const byte_t APP4 = 0xE4;
const byte_t APP5 = 0xE5;
const byte_t APP6 = 0xE6;
const byte_t APP7 = 0xE7;
const byte_t APP8 = 0xE8;
const byte_t APP9 = 0xE9;
const byte_t APP10 = 0xEA;
const byte_t APP11 = 0xEB;
const byte_t APP12 = 0xEC;
const byte_t APP13 = 0xED;
const byte_t APP14 = 0xEE;
const byte_t APP15 = 0xEF;

struct fileData {
    int quantizationTables[4][64] = {0}; // JPEG standard allows for upto 4 QTs
    int quantizationTablesSet[4] = {0}; // Set to 1 if that QT exists
    int SoFData[12] = {0};
    byte_t huffmanTables[2][4][162] = {0};
    int huffmanTableFreqs[2][4][16] = {0};
    int huffmanTableFreqCount[2][4] = {0};
    std::vector<byte_t> huffmanEncodedData;
};

fileData* readFile(const std::string &fileName);

#endif
