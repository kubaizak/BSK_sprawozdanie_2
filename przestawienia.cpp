#include <iostream>
#include <string>
#include <vector>
#include <numeric>
#include <functional>
#include <math.h>
#include <deque>
#include "przestawienia.hpp"

typedef std::string (*fcnPtr)(std::string, std::vector<int>);

static fcnPtr           executeFn = Przestawienia::encrypt;
static std::string      message;
static std::vector<int> key;

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
        if(param.size() == 2 && param[1] == 'd') executeFn = Przestawienia::decrypt;      //if -d then decrypt
        else throw -2; //std::runtime_error("Invalid option");

        params_list.pop_front();                                //either way it is flag so remove param
    }
    
    if(params_list.size() != 2) throw -1; //std::runtime_error("Invalid parameters");
    
    message = params_list.front();                              //cant be empty otherwise it would not be a param
    params_list.pop_front();  
    std::string key = params_list.front();
    
    std::string delimiter   = "-";
    size_t pos              = 0;
    std::string token;
    
    while(pos = key.find(delimiter) != std::string::npos){
        token = key.substr(0, pos);
        ::key.push_back(std::stoi(token));                      //may throw invalid_argument or out_of_range
        key.erase(0, pos + delimiter.size());
        
    }
    ::key.push_back(std::stoi(key));
    int sum = std::accumulate(::key.begin(), ::key.end(), 0, std::plus<int>());
    int n   = ::key.size();
    if(n < 1)                       throw -4;
    if(sum != (n * (n + 1) / 2))    throw -5;                   //checking if elements are consecutive
}

int main(int argc, char* argv[]){
    try{
        set_global_variables(argc, argv);
    }catch(std::invalid_argument e){
        std::cout << "Not a number argument!" << '\n';
        return -1;
    }catch(int e){
        std::string msg;

        switch(e){
            case  0: msg =  "Usage:\n"
                            "\t-encrypt: przestawienia <message> <key_1-...-key_n>\n"
                            "\t\twhere key_n in <key_1-...-key_n> must be consecutive number after sorting\n"
                            "\t-decrypt: przestawienia -d <encrypted_message> <key_1-...-key_n>"; 
                break;
            case -1: msg = "Invalid arguments!"; break;
            case -2: msg = "Invalid options!"; break;
            case -4: msg = "Invalid number of columns in key argument!"; break;
            case -5: msg = "Column indexes in key argument must be consecutive after sorting!"; break;
            default: msg = "WTF?";
        }

        std::cout << msg << '\n';
        return -1;
    }

    std::string out = executeFn(message, key) ;
    std::cout << out << std::endl;
    return 0;
}

namespace Przestawienia{

    std::string encrypt(std::string message, std::vector<int> key){
        //TODO: remove spaces from message
        
        int len ( message.size() );
        int columns ( key.size() );
        int rearranged_key[4] = {0};
        std::string encrypted(ceil((double)len / columns) * columns, ' ');

        //rearranging key, so we have 1 loop without nesting
        //key's value at index is the place in rearranged_key
        //EG. 3-1-4-2 -> 2-4-1-3
        for(int i{0}; i < columns; ++i){
            rearranged_key[key[i] - 1] = i;
        }

        for(int i {0}; i < len; ++i){
            int row_no { i / columns };
            int idx { row_no * columns + rearranged_key[i % columns] };
            encrypted[idx] = message[i];
        }

        return encrypted;
    }

    std::string decrypt(std::string encrypted, std::vector<int> key){
        int len ( encrypted.size() );
        int columns = ( key.size() );
        std::string message(len, ' ');

        for(int i {0}; i < len; ++i){
            int row_no { i / columns };
            int idx { row_no * columns + key[i % columns] - 1 };
            message[idx] = encrypted[i];
        }
        //TODO: remove spaces from message

        return message;
    }

    // std::string encrypt_old(std::string message, std::vector<int> key){
    //     int len = message.size();
    //     int columns = key.size();
    //     int rows = ceil((double)len / columns);
    //     std::string encoded;

    //     for(int i = 0; i < rows; i++){
    //         int row_no = i * columns;

    //         for(int k = 0; k < columns; k++){
    //             int idx = row_no + key[k] - 1;
    //             if(idx < len)
    //                 encoded += message[idx];
    //         }
    //     }

    //     return encoded;
    // }

}