#include <iostream>
#include <fstream>

using namespace std;

void decodeEXIF(ifstream &infile) {
    uint length = (infile.get() << 8) + infile.get();

    char buffer[6];
    char exifID[] = {'E', 'x', 'i', 'f', '\0', '\0'};
    infile.read(buffer, 6);
    cout << buffer << "\n";
    bool isExif = true;
    for (int i=0; i < 6; i++) {
        if (buffer[i] != exifID[i]) {
            isExif = false;
            cout << "asffa";
        }
    }
    if (isExif == true) {
        cout << "sDsad";
    }
}