//
//  FileManager.h
//  TopicosDev
//
//  Created by Alonso Diego Esteban Ramirez Sanchez on 10/28/18.
//  Copyright © 2018 Alonso Diego Esteban Ramirez Sanchez. All rights reserved.
//

#ifndef FileManager_h
#define FileManager_h

#include <iostream>
#include <fstream>
#include <sstream>

class FileManager{
private:
    std::fstream file;
public:
    FileManager(){}
    void createFile(std::string name){
        std::ofstream outfile (name);
    }
    bool fileOpen(std::string filePath){
        if(file.is_open()) return false;
        file.open(filePath);
        return file.good();
    }
    void fileClose(){
        file.close();
    }
    void writeLine(std::string line){
        if(!file.is_open()) return;
        file << line;
    }
    
    void writeMatrix(vvb matrix){
        for(ui i = 0; i < matrix.size(); i++){
            writeLine(std::to_string(matrix[i][0]));
            for(ui j = 1; j < matrix[i].size(); j++){
                writeLine("," + std::to_string(matrix[i][j]));
            }
            writeLine("\n");
        }
    }
    bool isNextLine(){
        return !file.eof();
    }
    std::string readLine(){
        std::string line;
        getline(file, line);
        return line;
    }
    std::vector<std::string> readFile(){
        std::vector<std::string> lines;
        while(isNextLine()){
            lines.push_back(readLine());
        }
        return lines;
    }
};

#endif /* FileManager_h */
