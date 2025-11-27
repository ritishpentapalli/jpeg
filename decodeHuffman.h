#ifndef DECODEHUFFMAN_H
#define DECODEHUFFMAN_H

#include <cstdint>


struct HuffmanCode {
    uint16_t code; // Huffman codes can be at most 16 bits long
    uint8_t length; // Length of the Huffman code in bits
};
/*
NOTE: The length field exists because two codes can have the same bit pattern but different lengths
For example, the codes '0101' and '101' are unique but will have the same value for their code field
*/
#endif