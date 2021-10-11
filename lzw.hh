//
// Created by tunc on 03/10/2021.
//

#pragma once

#include <climits>
#include <fstream>

class LZW {
  public:
    static const int STARTING_CODE_SIZE = CHAR_BIT+1; // 9, I want to include extended ASCII range, but
    // by letting code_size to be bigger than what is needed at the start, I will know when to increase it
    // Also, I know a byte isn't always 8 bits but I choose to ignore that
    static const int ALPHABET_SIZE = (1<<(STARTING_CODE_SIZE-1)); // (2^8)

    LZW();
    ~LZW();
    LZW(const LZW&) = delete;
    LZW(LZW&&) = delete;
    LZW& operator=(const LZW&) = delete;
    LZW& operator=(LZW&&) = delete;

    // Encode an input stream, and write to output stream
    static void encode(std::ifstream* is, std::ofstream* os);

    // Decode an input stream, and write to output stream
    static void decode(std::ifstream* is, std::ofstream* os);
};