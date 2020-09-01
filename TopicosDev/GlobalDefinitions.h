//
//  GlobalDefinitions.h
//  TopicosDev
//
//  Created by Alonso Diego Esteban Ramirez Sanchez on 11/1/18.
//  Copyright © 2018 Alonso Diego Esteban Ramirez Sanchez. All rights reserved.
//

#ifndef GlobalDefinitions_h
#define GlobalDefinitions_h
#include <iostream>
#include <vector>

template<typename T>
struct ResponseGroup {
    std::vector<T> res;
    std::vector<double> scores;
};
template<typename T, typename J, typename Z>
struct Triplete {
    T first;
    J second;
    Z third;
};

struct Factor {
    std::string name;
    std::vector<unsigned int> vars;
    std::vector<int> vals;
    std::vector<double> probs;
};


#endif /* GlobalDefinitions_h */
