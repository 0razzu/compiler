#ifndef globals_hpp
#define globals_hpp


#include <iostream>


// universal
const uint8_t OK = 0x0, MEM_ERR = 0x1;

// stack
const uint8_t EMPTY = 0x2;
const ushort StackBlockSize = 8;

// recognizer
const uint8_t READ_ERR = 0x4, UNEXPECTED = 0x8, BRACKETS = 0x10;


#endif
