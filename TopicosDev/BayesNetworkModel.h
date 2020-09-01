//
//  dataset.h
//  Topicos Clase 1
//
//  Created by Alonso Diego Esteban Ramirez Sanchez on 8/23/18.
//  Copyright © 2018 Alonso Diego Esteban Ramirez Sanchez. All rights reserved.
//

#ifndef dataset_h
#define dataset_h

#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include "Support.h"
#include "FileManager.h"

typedef std::vector<bool> vb;
typedef std::vector<std::vector<bool> > vvb;
typedef std::string str;
typedef std::vector<std::vector<std::vector<double> > > vvvd;
typedef std::vector<std::vector<double> > vvd;
typedef std::vector<double> vd;

class BayesNetworkModel{
private:
public:
    str name;
    vstr variables;
    vui varTypes;//0-> discrete, 1->continous
    vui cardinalities;
    vvstr values;
    vvui data;
    vvb dag;
    vvd mappedValues;
    double alpha;
    
    BayesNetworkModel(){
        alpha = 0.0f;
    }
    void ReadData(std::string filePath, bool header){
        FileManager fileManager = FileManager();
        if(!fileManager.fileOpen(filePath)){//Try to read file if error is thrown then print;
            std::cout << "Se detecto un error al abrir el archivo";
            return;
        }
        vstr lines;
        lines = fileManager.readFile();
        fileManager.fileClose();
        
        ui i = 0;
        if(header){
            variables = support::splitLine(lines[i], ",");
            i += 1;
        }
        
        for(; i < lines.size(); i++){
            vstr items = support::splitLine(lines[i], ",");
            data.push_back(vui());
            for(ui j = 0; j < items.size(); j++){
                int index = findValue(variables[j], items[j]);
                if(index > -1){
                    data[(header ? (i-1) : i)].push_back(index);
                }else{
                    std::cout << "Se encontro " << items[j] << " en el dataset, no fue declarado como valor" << std::endl;
                    data.clear();
                    return;
                }
            }
        }
        if(!data.size()){
            std::cout << "El archivo esta vacio" << std::endl;
            return;
        }
        std::cout << "Cabecera: " << ((header) ? "SI" : "NO") << std::endl;
        std::cout << "+" << data.size() << " instancias" << std::endl;
        std::cout << "+" << data[0].size() << " variables" << std::endl;
    }
    
    void addVariable(str var){
        variables.push_back(var);
        values.push_back(vstr());
        cardinalities.push_back(0);
        varTypes.push_back(0);
        std::cout << "+var(" << variables.size() - 1 << "): " << var << std::endl;
    }
    void addValue(ui varIndex, str val){
        values[varIndex].push_back(val);
        cardinalities[varIndex] += 1;
        std::cout << "+val(" << (values[varIndex].size()-1) << "): " << val << std::endl;
    }
    void addArc(ui var1, ui var2){
        dag[var2][var1] = 1;
    }
    
    void setAlpha(str _alpha){
        alpha = std::stod(_alpha);
        std::cout << "+alpha: " << _alpha << std::endl;
    }
    
    int findVariable(str var){
        for(ui i = 0; i < variables.size(); i++){
            if(!var.compare(variables[i])) return i;
        }
        return -1;
    }
    int findValue(str var, str val){
        int index = findVariable(var);
        if(index < 0) return -1;
        for(ui i = 0; i < values[index].size(); i++){
            if(!val.compare(values[index][i])) return i;
        }
        return -1;
    }
    
    void printCardinalities(){
        for(ui i = 0; i < cardinalities.size(); i++){
            std::cout << "+var(" << i << "): " << cardinalities[i] << std::endl;
        }
    }
    void printDAG(){
        unsigned int horizontalOffset = 0;
        for(unsigned int i = 0; i < variables.size(); i++){
            if(variables[i].size() > horizontalOffset) horizontalOffset = variables[i].size();
        }
        for(ui i = 0; i < (horizontalOffset+1); i++){
            std::cout << " ";
        }
        for(ui i = 0; i < variables.size(); i++){
            std::cout << variables[i] << " ";
        }
        std::cout << std::endl;
        for(ui i = 0; i < variables.size(); i++){
            std::cout << variables[i];
            for(ui j = 0; j < (horizontalOffset-variables[i].size()+1); j++){
                std::cout << " ";
            }
            for(ui j = 0; j < variables.size(); j++){
                for(ui k = 0; k < (variables[j].size()/2); k++){
                    std::cout << " ";
                }
                std::cout << dag[i][j];
                for(ui k = 0; k < (variables[j].size()/2); k++){
                    std::cout << " ";
                }
                if(variables[j].size()%2) std::cout << " ";
            }
            std::cout << std::endl;
        }
    }
    void printVars(){
        support::print(variables);
        support::nextLine();
    }
    
    void Clear(){
        variables.clear();
        std::cout << "Variables cleared" << std::endl;
        cardinalities.clear();
        values.clear();
        std::cout << "Values cleared" << std::endl;
        data.clear();
        std::cout << "Data cleared" << std::endl;
        dag.clear();
        std::cout << "DAG cleared" << std::endl;
        alpha = 0;
        std::cout << "Alpha set to default (0)" << std::endl;
    }
};

#endif /* dataset_h */
