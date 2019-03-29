#include <iostream>
#include <deque>
#include <string>
#include "rail_fence.hpp"

typedef std::string (*fcnPtr)(std::string, int);

static fcnPtr       executeFn = RailFence::encrypt;
static std::string  message;
static int          n;

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
        if(param.size() == 2 && param[1] == 'd') executeFn = RailFence::decrypt;      //if -d then decrypt
        else throw -2; //std::runtime_error("Invalid option");

        params_list.pop_front();                                //either way it is flag so remove param
    }
    
    if(params_list.size() != 2) throw -1; //std::runtime_error("Invalid parameters");
    
    message = remove_substring(params_list.front(), " ");       //cant be empty otherwise it would not be a param
    params_list.pop_front();  
    n = std::stoi(params_list.front());                         //may throw invalid_argument or out_of_range
    if(n < 2) throw -4;
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
                            "\t-encrypt: rail_fence <message> <levels_count>\n"
                            "\t-decrypt: rail_fence -d <encrypted_message> <levels_count>"; 
                break;
            case -1: msg = "Invalid arguments!"; break;
            case -2: msg = "Invalid options!"; break;
            case -4: msg = "Invalid levels count argument!"; break;
            default: msg = "WTF?";
        }

        std::cout << msg << '\n';
        return -1;
    }

    std::string out = executeFn(message, n) ;
    std::cout << out << std::endl;
    return 0;
}

namespace RailFence{

    std::string encrypt(std::string message, int n){
        // message.erase(std::remove_if(message.begin(), message.end(), ' '), message.end());
        int len = message.size();
        int cycle_len = 2*(n - 1);
        int cycles_count = len / cycle_len;
        std::string result;
        int idx;

        for(int i = 0; i < n; i++){
            for (int c = 0; c <= cycles_count; c++){
                idx = i + c * cycle_len;

                if(idx < len)
                    result += message[idx];
                else break;

                if(0 < i && i < n - 1){
                    idx = (c+1)*cycle_len - i;

                    if(idx < len)
                        result += message[idx];
                }
                    
            }
        }

        return result;
    }

    std::string decrypt(std::string encrypted, int n){
        int len = encrypted.size();
        int cycle_len = 2*(n - 1);
        int cycles_count = len / cycle_len;
        std::string decrypted (len, ' ');
        int idx;    
        int pos = 0;

        for(int i = 0; i < n; i++){
            for (int c = 0; c <= cycles_count; c++){
                idx = i + c * cycle_len;

                if(idx < len)
                    decrypted[idx] = encrypted[pos++];
                else break;

                if(0 < i && i < n - 1){
                    idx = (c+1)*cycle_len - i;

                    if(idx < len)
                        decrypted[idx] = encrypted[pos++];
                }
                    
            }
        }

        return decrypted;
    }

    //    levels\n  0123456789
    //       0      \   X   X
    //       1       \ / \ / \
    //       2        X   X
    // 1st cycle_half = cycle_idx
    // 2nd cycle_half = cycle_len - cycle_idx
    // F(cycle_idx) = [cycle_half_no] * cycle_len + [multiplier]*(-1) * cycle_idx

    void zigzag(int n, int levels){
        int cycle_len = 2 * (levels - 1);
        int cycle_half = cycle_len >> 1;

        // (0 < x) - (x < 0) -> -1 / 0 / 1
        // copysign(1.0, value)   (1.0, 2) -> 1 / (1.0, -2) -> -1

        for(int x = 0; x < n; ++x){
            int cycle_idx = x % cycle_len;
            int cycle_half_no = cycle_idx / cycle_half;
            int multiplier = cycle_idx < cycle_half ? -1 : 1;      //equivalent of copysign(1, x - cycle_half);
            int y = cycle_half_no * cycle_len + multiplier * (-1) * cycle_idx;
            std::cout << '(' << x << ',' << y << ')';
        }

        std::cout << std::endl;
    }

    // encrypt with zigzag
    // std::string encrypt_rail_fencexx(std::string message, int n){
    //     // message.erase(std::remove_if(message.begin(), message.end(), ' '), message.end());
    //     int len = message.size();
    //     int cycle_len = 2*(n - 1);
    //     int cycles_count = len / cycle_len;
    //     int divider = cycle_len / 2;
    //     std::string encrypted;

    //     // (0 < x) - (x < 0) -> -1 / 0 / 1
    //     // copysign(1.0, value)   (1.0, 2) -> 1 / (1.0, -2) -> -1

    //     for(int i = 0; i < len; ++i){
    //         // int cycle_idx = i / cycle_len * cycles_count;
    //         // int in_cycle_idx = i % cycle_len;
    //         // int idx = in_cycle_idx;
    //         int cycle_idx = i % cycle_len;


    //         encrypted[i] = message[i];
    //     }

    //     return encrypted;
    //     // for(int i = 0; i < n; i++){
    //     //     for (int c = 0; c <= cycles_count; c++){
    //     //         idx = i + c * cycle_len;

    //     //         if(idx < len)
    //     //             result += message[idx];
    //     //         else
    //     //             break;

    //     //         if(0 < i && i < n - 1){
    //     //             idx = (c+1)*cycle_len - i;

    //     //             if(idx < len)
    //     //                 result += message[idx];
    //     //         }
                    
    //     //     }
    //     // }

    //     // return result;
    // }
    
    void test(){
        int extreme = 3;

        for(int x = 0; x < 12; ++x){
            int tmp = (x % extreme);
            int y = x / extreme % 2 == 0 ? tmp : extreme - tmp;
            std::cout << '(' << x << ',' << y << ')';
        }
    }

    void print_characters(){
        for(int i = 0; i < 256; i++){
            std::cout << i << ": " << char(i) << std::endl;
        }
    }
}