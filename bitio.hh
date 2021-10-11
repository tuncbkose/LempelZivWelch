/*
 * Reused from 221, implementation written by me.
 */

#pragma once

#include <iostream>

class BitIO {
 public:
  // Construct with one of either an input stream or output (the other null)
  BitIO(std::ostream* os, std::istream* is);

  // Flushes out any remaining output bits and trailing zeros, if any:
  ~BitIO();

  BitIO(const BitIO&) = default;
  BitIO(BitIO&&) = default;
  BitIO& operator=(const BitIO&) = default;
  BitIO& operator=(BitIO&&) = default;

  // Output a single bit (buffered)
  void output_bit(bool bit);

  // Read a single bit (or trailing zero)
  bool input_bit();

  // Added for LZW
  // Returns if the input file reached EOF
  bool reached_eof();
 
 private:
  char buffer_; // contains the byte to be written/read
  int bit_index; // next empty value
  std::ostream* os_;
  std::istream* is_;
};
