#include <iostream>
#include <string>
#include <deque>
#include "vigenere.hpp"
#include "constants.hpp"

typedef std::string (*fcnPtr)(std::string, std::string);

static fcnPtr       executeFn = Vigenere::encrypt;
static std::string  message;
static std::string  key;

std::string remove_substring(std::string str, std::string sub){
    size_t pos = 0;
    
    while((pos = str.find(sub)) != std::string::npos){
        str.erase(pos, sub.size());
    }

    return str;
}

void set_global_variables(int argc, char** argv) {
    if(argc == 1)   throw 0;
    if(argc < 3)    throw -1; //std::runtime_error("Invalid parameters");

    std::deque<std::string> params_list;
    std::string param;

    for(int i {1}; i < argc; ++i){
        params_list.push_back(std::string(argv[i]));
    }
    
    param = params_list.front();

    if(param[0] == '-'/* && param.size() == 2*/){               //check if option flag
        if(param.size() == 2 && param[1] == 'd') executeFn = Vigenere::decrypt;      //if -d then decrypt
        else throw -2; //std::runtime_error("Invalid option");

        params_list.pop_front();                                //either way it is flag so remove param
    }
    
    if(params_list.size() != 2) throw -1; //std::runtime_error("Invalid parameters");
    
    message = remove_substring(params_list.front(), " ");       //cant be empty otherwise it would not be a param
    params_list.pop_front();  
    key = remove_substring(params_list.front(), " ");
}

int main(int argc, char* argv[]){
    try{
        set_global_variables(argc, argv);
    }catch(int e){
        std::string msg;

        switch(e){
            case  0: msg =  "Usage:\n"
                            "\t-encrypt: vigenere <message> <key_message>\n"
                            "\t-decrypt: vigenere -d <encrypted_message> <key_message>"; 
                break;
            case -1: msg = "Invalid arguments!"; break;
            case -2: msg = "Invalid options!"; break;
            default: msg = "WTF?";
        }

        std::cout << msg << '\n';
        return -1;
    }

    std::string out = executeFn(message, key) ;
    std::cout << out << std::endl;
    return 0;
}

namespace Vigenere{

    std::string encrypt(std::string message, std::string key){
        //c = E(m) = (m + k) mod n
        int n = Constants::ascii_max_range - Constants::ascii_min_range + 1;
        int key_len = key.size();
        std::string encrypted;

        for(int i = 0; i < message.size(); ++i){
            int m = (int)message[i] - Constants::ascii_min_range;
            int k = (int)key[i % key_len] - Constants::ascii_min_range;
            int c = (m + k) % n;
            encrypted += (char) (c + Constants::ascii_min_range);
        }

        return encrypted;
    }

    std::string decrypt(std::string encrypted, std::string key){
        //m = D(c) = (c - k) mod n
        int n = Constants::ascii_max_range - Constants::ascii_min_range + 1;
        int key_len = key.size();
        std::string message;

        for(int i = 0; i < encrypted.size(); ++i){
            int c = (int)encrypted[i] - Constants::ascii_min_range;
            int k = (int)key[i % key_len] - Constants::ascii_min_range;
            int m = (c - k + n) % n;    //+n to deal with negatives
            message += (char) (m + Constants::ascii_min_range);
        }

        return message;
    }

}