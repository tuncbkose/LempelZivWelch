//
// Created by tunc on 04/10/2021.
//

#include <unordered_map>
#include <string>

#include "lzw.hh"
#include "bitio.hh"

// Constructor
LZW::LZW() = default;

// Destructor
LZW::~LZW() = default;

void LZW::encode(std::ifstream* is, std::ofstream* os) {
    // Create symbol to code table
    // To make this code work faster, I would probably want to optimize this table, and not use these strings as keys
    std::unordered_map<std::string, int> table;
    for (int i = 0; i < LZW::ALPHABET_SIZE; ++i){
        table[std::to_string(i)] = i;
    }

    int code_size = LZW::STARTING_CODE_SIZE;
    // Since the first code is 0, the next code is ALPHABET_SIZE
    int next_code = LZW::ALPHABET_SIZE;
    // When this is reached, we must increase the code size
    int next_increase_point = LZW::ALPHABET_SIZE<<1;
    BitIO bitio(os, nullptr);

    // K is used to read from is
    int K;
    // Used to not call to_string on K multiple times.
    std::string str_K;

    // Start encoding
    K = is->get();
    std::string w = std::to_string(K);

    // This is here instead of while(true) in case input has only 1 byte
    // K will be -1 at EOF
    while (K != -1){
        K = is->get();

        // get returns -1 if EOF
        if (K == -1){
            break;
        }

        // If our table is full, expand
        if (next_code == next_increase_point){
            code_size += 1;
            next_increase_point <<= 1; // multiply by 2
        }

        str_K = std::to_string(K);

        // If w+K is not new, build up w
        if (table.contains(w+","+str_K)){
            w += (","+str_K);
        }
        // New pattern
        else{
            int code = table[w];
            // write code_size many bits, from left to right
            // the first couple of bits might need to be 0 if code_size is longer than actual length of code
            for (int i = code_size-1; i>=0; --i){
                bitio.output_bit((code>>i)&1);
            }
            table[w+","+str_K] = next_code;
            next_code += 1;
            w = str_K;
        }
    }
    // Write the last input
    int code = table[w];
    for (int i = code_size-1; i>=0; --i){
        bitio.output_bit((code>>i)&1);
    }
}

// For decoding table
// Instead of storing arbitrary long strings, just point to what should come before
struct symbol_type{
    symbol_type* prev_symbol;
    char curr_symbol;
};

// Reads `code_size` many bits from bitio to get the correct code
// bitio reads bit by bit, so this function abstracts the repetition during decoding
static int read_code(int code_size, BitIO* bitio){
    int b = 0;
    for (int i = 0; i < code_size; ++i){
        b <<= 1;
        b |= bitio->input_bit();
    }
    return b;
}

// Returns the first symbol to be saved in `FINchar` during decoding
static char write_symbol_and_return_first(symbol_type symbol, std::ofstream *os){
    if (symbol.prev_symbol != nullptr){
        char first_symbol = write_symbol_and_return_first(*symbol.prev_symbol, os);
        os->put(symbol.curr_symbol);
        return first_symbol;
    }
    else{
        os->put(symbol.curr_symbol);
        return symbol.curr_symbol;
    }

}
void LZW::decode(std::ifstream *is, std::ofstream *os) {
    // Create symbol to code table
    std::unordered_map<int, symbol_type> table;
    for (int i = 0; i < LZW::ALPHABET_SIZE; ++i){
        symbol_type s{nullptr, (char)i};
        table[i] = s;
    }

    int code_size = LZW::STARTING_CODE_SIZE;
    int next_code = LZW::ALPHABET_SIZE;
    int next_increase_point = LZW::ALPHABET_SIZE<<1;
    BitIO bitio(nullptr, is);

    // First code decoding
    // I am assuming the file will be nonempty
    int code = read_code(code_size, &bitio);
    int OLDcode = code;
    int INcode;
    symbol_type symbol = table[code];
    os->put(symbol.curr_symbol);
    char FINchar = symbol.curr_symbol;

    // main decoding loop
    while (true){
        // if table has only 1 slot left, the next new code will fill that
        // in which case encoding would increase code size
        // so we must as well
        if (next_code == next_increase_point - 1){
            code_size += 1;
            next_increase_point <<= 1;
        }
        code = read_code(code_size, &bitio);

        // we can check for eof only after trying to read beyond what is in file
        // Since our codes are longer than a byte, this happens exactly when there is nothing more to read
        if (bitio.reached_eof()){
            break;
        }

        INcode = code;

        // If we haven't seen the code before
        // it must be KwKwK (Kw was before so this is KwK)
        if (!table.contains(code)){
            // write Kw
            code = OLDcode;
            symbol = table[code];
            FINchar = write_symbol_and_return_first(symbol, os);
            // add new code
            symbol_type next_symbol{&table[OLDcode], FINchar};
            table[next_code] = next_symbol;

            INcode = next_code;
            OLDcode = INcode;
            // write last K
            os->put(FINchar);
            next_code += 1;
        }
        else{
            // write the pattern matching previously seen code
            symbol = table[code];
            FINchar = write_symbol_and_return_first(symbol, os);

            // add new code
            symbol_type next_symbol{&table[OLDcode], FINchar};
            table[next_code] = next_symbol;
            next_code += 1;
            OLDcode = INcode;
        }
    }

}