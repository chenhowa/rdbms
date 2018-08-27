

#ifndef BLOCK_BUFFER_HPP
#define BLOCK_BUFFER_HPP

#include <string>
#include <memory>
#include <fstream>

using namespace std;

class BlockBuffer {
private:
    BlockBuffer();
    int start_byte;
    int end_byte;
    int count;
    int max_bytes;
    unique_ptr<char[]> buffer;
    fstream file;
public:
    BlockBuffer(int blocksize);

    bool isEmpty();
    bool isFull();
    int write(int num_bytes, char* dest);
    int write(ofstream f);
    int read(int num_bytes, char* src);
    int read(ifstream f);
    bool bufferEquals(int num_bytes, char* src);
    void print();
    bool isStart(int index);
    bool isEnd(int index);
    bool isCount(int index);
    void setFile(char* filename);
    bool isFileReady();
    bool isFileEmpty();
};

#endif //BLOCK_BUFFER_HPP