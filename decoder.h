#ifndef DECODER_H
#define DECODER_H

typedef unsigned char byte_t;

// JPEG Markers

const byte_t PREFIX = 0xFF;

const byte_t SOI = 0xD8; // Start Of Image
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

const byte_t DQT = 0xDB; // Define Quantization Table

const byte_t SOF0 = 0xC0; // Baseline DCT
const byte_t SOF1 = 0xC1; // Extended sequential DCT
const byte_t SOF2 = 0xC2; // Progressive DCT
const byte_t SOF3 = 0xC3; // Lossless (sequential)
const byte_t SOF5 = 0xC5; // Differential sequential DCT
const byte_t SOF6 = 0xC6; // Differential progressive DCT
const byte_t SOF7 = 0xC7; // Differential lossless (sequential)
const byte_t SOF9 = 0xC9; // Extended sequential DCT, arithmetic coding
const byte_t SOF10 = 0xCA; // Progressive DCT, arithmetic coding
const byte_t SOF11 = 0xCB; // Lossless (sequential), arithmetic coding
const byte_t SOF13 = 0xCD; // Differential sequential DCT, arithmetic coding
const byte_t SOF14 = 0xCE; // Differential progressive DCT, arithmetic coding
const byte_t SOF15 = 0xCF; // 	Differential lossless (sequential), arithmetic coding

const byte_t DHT = 0xC4; // Define Huffman Table
const byte_t SOS = 0xDA; // Define Huffman Table

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

#endif
