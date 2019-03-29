#pragma once
#include <string>

namespace Caesar{

    int gcd(int, int);
    int gcd_extended(int, int, int*, int*);
    int mod_inverse(int, int);

    // Affine
    std::string encrypt(std::string, int, int);
    std::string decrypt(std::string, int, int);

}