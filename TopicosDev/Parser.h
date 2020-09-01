//
//  parser.h
//  TopicosDev
//
//  Created by Alonso Diego Esteban Ramirez Sanchez on 9/16/18.
//  Copyright © 2018 Alonso Diego Esteban Ramirez Sanchez. All rights reserved.
//

#ifndef parser_h
#define parser_h

#include <iostream>
#include <string>
#include <vector>
#include "Support.h"
#include "GlobalDefinitions.h"

namespace parser {
    ResponseGroup<std::vector<std::string> > ReadInputs(){
        ResponseGroup<std::vector<std::string> > response = ResponseGroup<std::vector<std::string> >();
        std::string line;
        std::getline (std::cin, line);
        response.res.push_back(support::splitLine(line, " "));
        response.scores.push_back((line.find("quit") == std::string::npos));
        return response;
    }
}

#endif /* parser_h */
