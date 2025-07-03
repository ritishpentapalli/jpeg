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
#endif
