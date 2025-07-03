#include "decoder.h"
#include "decodeAPPn.h"
#include <iostream>
#include <fstream>

using namespace std;

void decodeAPPnSegment(ifstream &infile, byte_t current) {
    
    uint length = (infile.get() << 8) + infile.get();
    for (int i=0; i<length-2; i++) { // length-2 because we already read two bytes when reading APPn marker
        infile.get();
    }
    // if (current == APP1) {
    //     cout << "APP1 found\n";
    //     decodeEXIF(infile);
    // }
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
            decodeAPPnSegment(infile, current);
            readAllAPPn = false;
        }

    }

    // Confirm that next marker is DQT
    if (previous != PREFIX || current != DQT) {
        cout << "Error - Did not find DQT marker\n";
        infile.close();
        return 1;
    }







    return 0;
}
