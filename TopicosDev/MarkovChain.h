//
//  MarkovChain.h
//  TopicosDev
//
//  Created by Alonso Diego Esteban Ramirez Sanchez on 9/18/18.
//  Copyright © 2018 Alonso Diego Esteban Ramirez Sanchez. All rights reserved.
//

#ifndef MarkovChain_h
#define MarkovChain_h

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>

typedef std::vector<double> vd;
typedef std::vector<std::vector<double> > vvd;
typedef std::vector<std::string> vstr;
typedef unsigned int ui;
typedef std::string str;

class MarkovChain{
public:
    vstr variables;
    vd initialState;
    vvd transitionMatrix;
    vd tState;
    ui t;
    
    MarkovChain(){
        variables = {"S", "N", "L"};
        for(ui i = 0; i < variables.size(); i++){
            transitionMatrix.push_back({});
            initialState.push_back(0);
            for(ui j = 0; j < variables.size(); j++){
                transitionMatrix[i].push_back(0);
            }
        }
        readData();
        t = 0;
        tState = initialState;
        printDistribution();
        predict();
        printDistribution();
        setTransition("S");
        printDistribution();
        setTransition("L");
        printDistribution();
    }
    int findVariable(str var){
        for(ui i = 0; i < variables.size(); i++){
            if(!var.compare(variables[i])) return i;
        }
        return -1;
    }
    void readData(){
        std::ifstream file("markovdata.csv");
        if(!file.is_open()){
            std::cout << "No se encontro el archivo" << std::endl;
            return;
        }
        std::string line, c;
        ui instances = 0;
        for(ui i = 0; getline(file,line); i++){
            c.clear();
            std::stringstream ss(line);
            std::string lastC;
            for(ui j = 0; getline(ss,c,','); j++){
                int index = findVariable(c);
                if(j == 0){
                    initialState[index] += 1;
                }else{
                    ui lastIndex = findVariable(lastC);
                    transitionMatrix[lastIndex][index] += 1;
                }
                lastC = c;
            }
            instances += 1;
        }
        for(ui i = 0; i < initialState.size(); i++){
            initialState[i] /= instances;
        }
        for(ui i = 0; i < variables.size(); i++){
            ui max = 0;
            for(ui j = 0; j < variables.size(); j++){
                max += transitionMatrix[i][j];
            }
            for(ui j = 0; j < variables.size(); j++){
                transitionMatrix[i][j] /= max;
            }
        }
    }
    vvd generateMatrix(ui _i, ui _j){
        vvd matrix;
        for(ui i = 0; i < _i; i++){
            matrix.push_back(vd());
            for(ui j = 0; j < _j; j++) matrix[i].push_back(0);
        }
        return matrix;
    }
    vvd multiplyMatrix(vvd matrix1, vvd matrix2){
        ui m = matrix1.size();
        ui n = matrix2[0].size();
        vvd matrix = generateMatrix(m, n);
        for(ui i = 0; i < m; i++){
            for(ui j = 0; j < n; j++){
                for(ui k = 0; k < matrix[0].size(); k++){
                    matrix[i][j] += (matrix1[i][k]*matrix2[k][j]);
                }
            }
        }
        return matrix;
    }
    void distributionT(ui t){
        vvd matrix = vvd({initialState});
        for(ui i = 0; i < t; i++) matrix = multiplyMatrix(matrix, transitionMatrix);
        for(ui i = 0; i < matrix.size(); i++){
            std::cout << "[ ";
            for(ui j = 0; j < matrix[i].size(); j++){
                std::cout << matrix[i][j] << " ";
            }
            std::cout << "]" << std::endl;
        }
    }
    void printDistribution(){
        std::cout << "[ ";
        for(ui i = 0; i < tState.size(); i++){
            std::cout << variables[i] << "="<< tState[i] << ", ";
        }
        std::cout << "]" << std::endl;
    }
    void setTransition(str var){
        ui index = findVariable(var);
        vd temp;
        for(ui i = 0; i < variables.size(); i++){
            temp.push_back(0);
        }
        temp[index] = 1;
        tState = multiplyMatrix({temp}, transitionMatrix)[0];
    }
    void iterateT(){
        tState = multiplyMatrix({tState}, transitionMatrix)[0];
    }
    str predict(){
        iterateT();
        ui index = 0;
        for(ui i = 1; i < tState.size(); i++){
            if(tState[i] > tState[index]) index = i;
        }
        std::cout << "El siguiente elemento de la cadena será " << variables[index] << std::endl;
        return variables[index];
    }
};

#endif /* MarkovChain_h */
