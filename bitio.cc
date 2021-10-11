/*
 * A simple class to perform stream I/O on individual bits.
 */
#include "bitio.hh"
#include <cassert>
#include <climits>

// Construct with one of either an input stream or output (the other null)
BitIO::BitIO(std::ostream* os, std::istream* is){

    //Assertion that only 1 argument is nullptr
    assert((os == nullptr || is == nullptr) && !(os == nullptr && is == nullptr));
    is_ = is;
    os_ = os;
    bit_index = 0;
    buffer_ = 0;
}


// Flushes out any remaining output bits and trailing zeros, if any:
BitIO::~BitIO(){

    if(os_){
        if(0<bit_index){
            // The output_bit function guarantees that buffer_ will have 0s
            // in bits not yet written.
            (*os_).put(buffer_);
        }
    }
}

// Output a single bit (buffered)
void BitIO::output_bit(bool bit){

    // Assert that object is for writing
    assert(is_ == nullptr);

    // Clear the buffer from bits to prevent potential accidents if first 8 calls are 0
    // and clear buffer_ every byte cycle.
    if (bit_index == 0){
        buffer_ = 0;
    }

    // case true is for writing bit in correct position
    // case false is for the general action of incrementing index
    switch (bit){
        case true:{ buffer_ |= (1<<bit_index);
                    [[fallthrough]];
        }
        case false: ++bit_index;
    }

    // if a byte is complete, output it to the stream
    // resetting buffer is delegated to the next call of the function.
    if (bit_index == CHAR_BIT){
        /* for debugging
        std::string binary = std::bitset<CHAR_BIT>(buffer_).to_string();
        std::cout<<"buffer_: " << binary << " , sent to os\n";
        */
        (*os_).put(buffer_);
        bit_index = 0;
    }
}

// Read a single bit (or trailing zero)
bool BitIO::input_bit(){

    // Assert that object is for reading
    assert(os_ == nullptr);

    // bit_index when function is called will be 0 only in the first call
    if(bit_index == CHAR_BIT || bit_index == 0){
        bit_index = 0;
        (*is_).get(buffer_);
    }
    // post-increment index and return the bit
    return (buffer_>>bit_index++) & 1;
}

// Check if EOF is reached
bool BitIO::reached_eof() {

    // Assert that object is for reading
    assert(os_ == nullptr);

    return (*is_).eof();
}