#ifndef compiler_hpp
#define compiler_hpp


#include "globals.hpp"
#include "stack.hpp"
#include "stack.cpp"
#include <iostream>
#include <fstream>
#include <sstream>


uint8_t recognizer(std::istream &in, bool from_file, char &wrong_symbol, u_long &curr, u_long &curr_line, std::stringstream &rpn);

bool generate_code(std::stringstream &rpn, std::ostream &code);


#endif
