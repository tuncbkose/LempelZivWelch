//
// Created by tunc on 05/10/2021.
//

#include <iostream>
#include <fstream>
#include <chrono>
#include <filesystem>
namespace fs = std::filesystem;

#include "lzw.hh"


int main(int argc, char* argv[]){
    // Checking that there are more than 1 arguments
    if (argc < 2){
        std::cerr<<"At least 1 file argument is needed to decompress file.\n";
        return 1;
    }

    for (int i=1; i !=argc; ++i){
        std::cout<<"Decompressing "<<argv[i]<<"\n";
        // I am opening both streams as binary so we will directly get the bytes
        std::ifstream input (argv[i], std::ios::binary);

        // if file can't be opened, move on to the next argument
        if (!input.is_open()){
            std::cout << "Unable to open " << argv[i] << ".\n";
        }

        else{
            LZW decoder;
            std::ofstream output;
            fs::path inp = fs::current_path() / argv[i];
            auto input_size = fs::file_size(inp);
            std::cout << "Initial file size: " << input_size << " bytes\n";

            output.open(std::string(argv[i])+".decomp", std::ios::binary);
            auto start_time = std::chrono::high_resolution_clock::now();
            decoder.decode(&input, &output);
            auto end_time = std::chrono::high_resolution_clock::now();
            output.close();
            auto duration = duration_cast<std::chrono::milliseconds>(end_time - start_time);

            // output statistics
            fs::path out = fs::current_path() / (std::string(argv[i])+".decomp");
            auto output_size = fs::file_size(out);
            std::cout << "Decompressed file size: " << output_size << " bytes\n";
            std::cout << "Time taken to decompress: " << duration.count() << " milliseconds\n";
        }
    }

}