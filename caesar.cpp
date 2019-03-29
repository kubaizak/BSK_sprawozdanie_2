#include <iostream>
#include <deque>
#include <string>
#include "caesar.hpp"
#include "constants.hpp"

typedef std::string (*fcnPtr)(std::string, int, int);

static fcnPtr       executeFn = Caesar::encrypt;
static std::string  message;
static int          a;
static int          b;

std::string remove_substring(std::string str, std::string sub){
    size_t pos = 0;
    
    while((pos = str.find(sub)) != std::string::npos){
        str.erase(pos, sub.size());
    }

    return str;
}

void set_global_variables(int argc, char** argv) {
    if(argc == 1)   throw 0;
    if(argc < 4)    throw -1; //std::runtime_error("Invalid parameters");

    std::deque<std::string> params_list;
    std::string param;

    for(int i {1}; i < argc; ++i){
        params_list.push_back(std::string(argv[i]));
    }
    
    param = params_list.front();

    if(param[0] == '-'/* && param.size() == 2*/){               //check if option flag
        if(param.size() == 2 && param[1] == 'd') executeFn = Caesar::decrypt;      //if -d then decrypt
        else throw -2; //std::runtime_error("Invalid option");

        params_list.pop_front();                                //either way it is flag so remove param
    }
    
    if(params_list.size() != 3) throw -1; //std::runtime_error("Invalid parameters");
    
    message = remove_substring(params_list.front(), " ");       //cant be empty otherwise it would not be a param
    params_list.pop_front();  
    a = std::stoi(params_list.front());                         //may throw invalid_argument or out_of_range
    params_list.pop_front();
    b = std::stoi(params_list.front());

    if(a < 1 || b < 1)    throw -4;
}

int main(int argc, char* argv[]){
    std::string out;
    try{
        set_global_variables(argc, argv);
        out = executeFn(message, a, b);
        std::cout << out << std::endl;
    }catch(std::invalid_argument e){
        std::cout << "Not a number argument!" << '\n';
        return -1;
    }catch(int e){
        std::string msg;

        switch(e){
            case  0: msg =  "Usage:\n"
                            "\t-encrypt: caesar <message> <k1> <k2>\n"
                            "\t\twhere k1 and k2 are coprime numbers\n"
                            "\t-decrypt: caesar -d <encrypted_message> <k1> <k2>"; 
                break;
            case -1: msg = "Invalid arguments!"; break;
            case -2: msg = "Invalid options!"; break;
            case -3: msg = "Not a coprime number!"; break;
            case -4: msg = "k1 and k2 must be coprime and > 0 numbers!"; break;
            default: msg = "WTF?";
        }

        std::cout << msg << '\n';
        return -1;
    }
    
    return 0;
}

namespace Caesar{

    int gcd(int a, int b){
        while(b != 0){
            int t = b;
            b = a % b;
            a = t;
        }

        return a;
    }

    // https://www.geeksforgeeks.org/multiplicative-inverse-under-modulo-m/
    // modular inverse (I): (a * I) % m == 1
    int mod_inverse(int a, int m){
        int x, y;
        int g = gcd_extended(a, m, &x, &y);

        if(g != 1) throw -3; //std::runtime_error("");

        // m is added to handle negative x 
        return (x % m + m) % m; 
    }

    int gcd_extended(int a, int b, int *x, int *y) 
    { 
        // Base Case 
        if (a == 0) 
        { 
            *x = 0, *y = 1; 
            return b; 
        } 
    
        int x1, y1; // To store results of recursive call 
        int gcd = gcd_extended(b%a, a, &x1, &y1); 
    
        // Update x and y using results of recursive call 
        *x = y1 - (b/a) * x1; 
        *y = x1; 
    
        return gcd; 
    } 

    std::string encrypt(std::string message, int a, int b){
        /*
            Characters range [33,126] -> [MIN_RANGE,MAX_RANGE]
            E(x) = (a*x + b) mod n
            a, b - relatively prime (coprime)
            n - size of the character range
        */

        if(gcd(a, b) != 1) throw -3; //std::runtime_error("");    //NOT coprime

        int n = Constants::ascii_max_range - Constants::ascii_min_range + 1;
        std::string encrypted;    

        for(int i = 0; i < message.size(); ++i){
            int x = (int)message[i] - Constants::ascii_min_range;
            x = (a * x + b) % n;
            encrypted += (char)(x + Constants::ascii_min_range);
            // encrypted += (char)((((int)message[i] - MIN_RANGE) * k1 + k2) % n + MIN_RANGE);
        }    

        return encrypted;
    }

    std::string decrypt(std::string encrypted, int a, int b){
        /*
            Characters range [33,126] -> [MIN_RANGE,MAX_RANGE]
            D(x) = a^1 * (x - b) mod n
            a^1 - modular multiplicative inverse
            a, b - relatively prime (coprime)
            n - size of the character range
        */
        
        if(gcd(a, b) != 1) throw -3;
        
        int n = Constants::ascii_max_range - Constants::ascii_min_range + 1;
        int I = mod_inverse(a, n);
        std::string decrypted;

        for(int i = 0; i < encrypted.size(); ++i){
            int x = (int)encrypted[i] - Constants::ascii_min_range;
            //application of modular multiplication in order to deal with negatives
            // x = (I * (x - b)) % n;
            x = (I % n) * ((x + n - b) % n) % n;    //+n is to deal with negative modulo
            decrypted += (char)(x + Constants::ascii_min_range);
        }

        return decrypted;
    }

}