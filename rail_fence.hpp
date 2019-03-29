#ifndef RAIL_FENCE_HPP
#define RAIL_FENCE_HPP

#include <string>

namespace RailFence{

    std::string encrypt(std::string, int);
    std::string decrypt(std::string, int);

    void zigzag(int n, int levels);

}

#endif