//
//  support.h
//  TopicosDev
//
//  Created by Alonso Diego Esteban Ramirez Sanchez on 9/6/18.
//  Copyright © 2018 Alonso Diego Esteban Ramirez Sanchez. All rights reserved.
//

#ifndef support_h
#define support_h

#include <iostream>
#include <vector>
#include <string>
#include <math.h>

typedef unsigned int ui;
typedef std::vector<unsigned int> vui;
typedef std::vector<std::vector<unsigned int> > vvui;
typedef std::vector<std::vector<double> > vvd;
typedef std::vector<std::string> vstr;
typedef std::vector<std::vector<std::string> > vvstr;
typedef std::vector<unsigned char> vuc;
typedef std::vector<std::vector<bool> > vvb;
typedef std::vector<bool> vb;

namespace support {
    void expand(ui n, vui& temp){
        for(ui i = 0; i < n; i++)
            temp.push_back(0);
    }
    template<typename T>
    int find(std::vector<T> vec, T elem){
        for(ui i = 0; i < vec.size(); i++)
            if(vec[i] == elem) return i;
        return -1;
    }
    template<typename T>
    std::vector<T> slice(std::vector<T> vec, ui start, ui end){
        std::vector<T> res;
        for(ui i = start; i < end; i++){
            res.push_back(vec[i]);
        }
        return res;
    }
    template<typename T>
    std::vector<T> merge(std::vector<T> a, std::vector<T> b){
        std::vector<T> c = a;
        for(ui i = 0; i < b.size(); i++){
            c.push_back(b[i]);
        }
        return c;
    }
    ui factoreSize(vui vars, vui cardinalidad){
        if(vars.size() == 0)return 0;
        ui fact = 1.0f;
        for(ui i = 0; i < vars.size(); i++){
            fact *= cardinalidad[vars[i]];
        }
        return fact;
    }
    template<typename T>
    void print(std::vector<T> toPrint){
        for(ui i = 0; i < toPrint.size(); i++){
            std::cout << toPrint[i] << " ";
        }
    }
    void nextLine(){
        std::cout << std::endl;
    }
    template<typename T>
    double sum(std::vector<T> vec){
        double res = 0.0f;
        for(ui i = 0; i < vec.size(); i++){
            res += vec[i];
        }
        return res;
    }
    vui findParents(vb dag){
        vui res;
        for(ui i = 0; i < dag.size(); i++)
            if(dag[i] == 1) res.push_back(i);
        return res;
    }
    vvb genMatrix(ui _i, ui _j){
        vvb res;
        for(ui i = 0; i < _i; i++){
            res.push_back(vb());
            for(ui j = 0; j < _j; j++){
                res[i].push_back(0);
            }
        }
        return res;
    }
    bool _isDAG(vvb dag, vb visited, ui current){
        for(ui i = 0; i < dag[current].size(); i++){
            if(current == i) continue;
            if(visited[i] && dag[current][i]){
                return false;
            }
            if(dag[current][i]){
                visited[i] = true;
                if(!_isDAG(dag, visited, i)) return false;
                visited[i] = false;
            }
        }
        return true;
    }
    bool isDAG(vvb dag){
        for(ui i = 0; i < dag.size(); i++){
            vb visited;
            for(ui j = 0; j < dag.size(); j++){
                visited.push_back(false);
            }
            visited[i] = true;
            if(!_isDAG(dag, visited, i)) return false;
        }
        return true;
    }
    vvui allValuesCombinations(vui variables, vui cardinalidades){
        vvui combs;
        ui tamFactor = support::factoreSize(variables, cardinalidades);
        for(ui i = 0; i < tamFactor; i++){
            ui t = tamFactor;
            vui comb;
            for(ui j = 0; j < variables.size(); j++){
                t /= cardinalidades[variables[j]];
                comb.push_back((i/t) % cardinalidades[variables[j]]);
            }
            combs.push_back(comb);
        }
        return combs;
    }
    template<typename T>
    void combinationUtil(std::vector<T> arr, std::vector<T> data, ui start, ui end, ui index, ui r, std::vector<std::vector<T> >& combs)
    {
        if (index == r)
        {
            combs.push_back(data);
            return;
        }
        
        for (int i=start; i<=end && end-i+1 >= r-index; i++)
        {
            data[index] = arr[i];
            combinationUtil(arr, data, i+1, end, index+1, r, combs);
        }
    }
    template<typename T>
    std::vector<std::vector<T> > combineArray(std::vector<T> arr){//167 183
        std::vector<std::vector<T> > res;
        double max = pow(2, arr.size());
        bool b[arr.size()];
        for(ui i = 0; i < max; i++){
            res.push_back(std::vector<T>());
            //b = b << 1;
            for(ui j = 0; j < arr.size(); j++){
                std::cout << b[j] << " ";
                if(b[j] == 1){
                    res[i].push_back(arr[j]);
                }
            }
            std::cout << std::endl;
        }
        return res;
    }
    vstr splitLine(std::string line, std::string delimiter){
        vstr data;
        size_t pos = 0;
        while ((pos = line.find(delimiter)) != std::string::npos) {
            data.push_back(line.substr(0, pos).c_str());
            line.erase(0, pos + delimiter.length());
        }
        if(line.size() > 0) data.push_back(line);
        return data;
    }
}

#endif /* support_h */
