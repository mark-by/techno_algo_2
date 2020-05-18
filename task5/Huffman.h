#ifndef _HUFFMAN_H
#define _HUFFMAN_H

typedef unsigned char byte;

struct IInputStream {
    virtual bool Read(byte &value) = 0;
};

struct IOutputStream {
    virtual void Write(byte &value) = 0;
};

#endif //_HUFFMAN_H
