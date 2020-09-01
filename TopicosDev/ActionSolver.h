//
//  actions.h
//  TopicosDev
//
//  Created by Alonso Diego Esteban Ramirez Sanchez on 9/16/18.
//  Copyright © 2018 Alonso Diego Esteban Ramirez Sanchez. All rights reserved.
//

#ifndef actions_h
#define actions_h

#include <stdlib.h>
#include <sstream>
#include "FileManager.h"

namespace actionsolver {
    
    //Quit program
    void ExitSolver(){
        exit(0);
    }
    void CreateModelSolver(vstr data, std::vector<BayesNetworkModel*>& models){
        if(data.size() < 2){
            std::cout << "No se declaró nombre de modelo" << std::endl;
            return;
        }
        BayesNetworkModel* model = new BayesNetworkModel();
        for(unsigned int i = 0; i < models.size(); i++){
            if(!models[i]->name.compare(data[1])){
                models[i] = model;
                break;
            }
        }
        model->name = data[1];
        std::cout << "Creado modelo " << model->name << std::endl;
        models.push_back(model);
    }
    void SelectSolver(vstr data, std::vector<BayesNetworkModel*> models, BayesNetworkModel*& current){
        if(data.size() < 2){
            std::cout << "No se declaró nombre de modelo" << std::endl;
            return;
        }
        for(ui i = 0; i < models.size(); i++){
            if(models[i]->name.compare(data[1]) == 0){
                current = models[i];
                std::cout << "Seleccionado modelo " << current->name << std::endl;
                
                return;
            }
        }
        std::cout << "No se encontró modelo con ese nombre" << std::endl;
    }
    //Set variables
    void VarsSolver(vstr data, BayesNetworkModel* model){
        if(model == NULL){
            std::cout << "No se ha seleccionado un modelo" << std::endl;
            return;
        }
        model->variables.clear();
        model->values.clear();
        model->varTypes.clear();
        model->cardinalities.clear();
        for(ui i = 1; i < data.size(); i++){
            model->addVariable(data[i]);
        }
        model->dag = support::genMatrix(model->variables.size(), model->variables.size());
    }
    //Set variable values
    void ValsSolver(vstr data, BayesNetworkModel* model){
        if(model == NULL){
            std::cout << "No se ha seleccionado un modelo" << std::endl;
            return;
        }
        if(data.size() < 3){ std::cout << "No hay suficientes parametros" << std::endl; return; }
        int varIndex = -1;
        varIndex = model->findVariable(data[1]);
        if(varIndex < 0){
            std::cout << "Variable no existe" << std::endl;
            return;
        }
        if(!data[2].compare("-c")){
            if(data.size() < 4){
                std::cout << "No se declaro numero de sub-grupos" << std::endl;
                return;
            }
            model->values[varIndex].clear();
            model->varTypes[varIndex] = 1;
            std::cout << "A la variable continua " << model->variables[varIndex] << " se declara los valores:" << std::endl;
            ui subGroups = std::stoi(data[3]);
            for(ui j = 0; j < subGroups; j++){
                std::stringstream ss;
                ss << j;
                model->addValue(varIndex, ss.str());
            }
        }else{
            model->values[varIndex].clear();
            std::cout << "A la variable dicreta " << model->variables[varIndex] << " se declara los valores:" << std::endl;
            for(ui i = 2; i < data.size(); i++){
                model->addValue(varIndex, data[i]);
            }
        }
    }
    //Set data path
    void DataSolver(vstr data, BayesNetworkModel* model){
        if(model == NULL){
            std::cout << "No se ha seleccionado un modelo" << std::endl;
            return;
        }
        std::cout << model->name << std::endl;
        if(model->variables.size() < 1){
            std::cout << "No se ha declarado variables para este modelo" << std::endl;
            return;
        }
        if(model->values.size() < 1){
            std::cout << "No se ha declarado valores para este modelo" << std::endl;
            return;
        }
        bool header = false;
        for(ui i = 1; i < data.size(); i++){
            if(!data[i].compare("-h")){
                data = support::merge(support::slice(data, 0, i), support::slice(data, i+1, data.size()));
                header = true;
                break;
            }
        }
        if(data.size() > 2){
            for(unsigned int i = 1; i < data.size() - 1; i++){
                data[i] += ("\ " + data[i+1]);
            }
            data = support::slice(data, 0, 2);
        }
        model->ReadData(data[1], header);
    }
    //Set dependency arc between two variables
    void ArcSolver(vstr data, BayesNetworkModel* model){
        if(model == NULL){
            std::cout << "No se ha seleccionado un modelo" << std::endl;
            return;
        }
        if(data.size() < 3) std::cout << "No hay suficientes parametros" << std::endl;
        int varIndex = -1;
        varIndex = model->findVariable(data[1]);
        if(varIndex < 0){
            std::cout << "Variable no existe" << std::endl;
            return;
        }
        for(ui i = 2; i < data.size(); i++){
            int index2 = model->findVariable(data[i]);
            if(index2 < 0){
                std::cout << "No existe la variable " << model->variables[index2] << std::endl;
                return;
            }
            model->addArc(varIndex, index2);
            std::cout << "Arco creado " << model->variables[varIndex] << "->" << model->variables[index2] << std::endl;
        }
    }
    //Show DAG
    void DagSolver(BayesNetworkModel* model){
        if(model == NULL){
            std::cout << "No se ha seleccionado un modelo" << std::endl;
            return;
        }
        model->printDAG();
    }
    //Set alpha value
    void AlphaSolver(vstr data, BayesNetworkModel* model){
        if(model == NULL){
            std::cout << "No se ha seleccionado un modelo" << std::endl;
            return;
        }
        model->setAlpha(data[1]);
    }
    //Show variables cardinlities
    void CardsSolver(BayesNetworkModel* model){
        if(model == NULL){
            std::cout << "No se ha seleccionado un modelo" << std::endl;
            return;
        }
        model->printCardinalities();
    }
    //Show marginal probability
    void MarginalProbSolver(vstr data, BayesNetworkModel* model){
        if(model == NULL){
            std::cout << "No se ha seleccionado un modelo" << std::endl;
            return;
        }
        if(data.size() == 4) engine::printMarginalProbabilityWithValue(data[1], data[3], model);
        else engine::printMarginalProbability(data[1], model);
    }
    //Show joint probability
    void JointProbSolver(vstr data, BayesNetworkModel* model){
        if(model == NULL){
            std::cout << "No se ha seleccionado un modelo" << std::endl;
            return;
        }
        vstr vars = support::slice(data, 1, data.size());
        engine::printJointProbability(vars, model);
    }
    //Show conditional probability
    void CondProbSolver(vstr data, BayesNetworkModel* model){
        if(model == NULL){
            std::cout << "No se ha seleccionado un modelo" << std::endl;
            return;
        }
        vstr vars;
        vstr parentVars;
        ui i = 1;
        for(; i < data.size(); i++){
            if(!data[i].compare("-p")) break;
            vars.push_back(data[i]);
        }
        for(i += 1; i < data.size(); i++){
            parentVars.push_back(data[i]);
        }
        engine::printConditionalProbability(vars, parentVars, model);
    }
    //Show results of marginalization of variables
    void MarginalizationSolver(vstr data, BayesNetworkModel* model){
        if(model == NULL){
            std::cout << "No se ha seleccionado un modelo" << std::endl;
            return;
        }
        vstr vars;
        vstr marginalizedVars;
        ui i = 1;
        for(; i < data.size(); i++){
            if(!data[i].compare("-m")) break;
            vars.push_back(data[i]);
        }
        for(i += 1; i < data.size(); i++){
            marginalizedVars.push_back(data[i]);
        }
        engine::printMarginalization(vars, marginalizedVars, model);
    }
    //Show Conditional Independence Test by Mutual Information
    void MutualInformationSolver(vstr data, BayesNetworkModel* model){
        if(model == NULL){
            std::cout << "No se ha seleccionado un modelo" << std::endl;
            return;
        }
        vstr vars;
        vstr parentVars;
        ui i = 1;
        for(; i < data.size(); i++){
            if(!data[i].compare("-p")) break;
            vars.push_back(data[i]);
        }
        for(i += 1; i < data.size(); i++){
            parentVars.push_back(data[i]);
        }
        engine::printMutualInformation(vars, parentVars, model);
    }
    //Show Conditional Independence Test by Pearson
    void PearsonSolver(vstr data, BayesNetworkModel* model){
        if(model == NULL){
            std::cout << "No se ha seleccionado un modelo" << std::endl;
            return;
        }
        vstr vars;
        vstr parentVars;
        ui i = 1;
        for(; i < data.size(); i++){
            if(!data[i].compare("-p")) break;
            vars.push_back(data[i]);
        }
        for(i += 1; i < data.size(); i++){
            parentVars.push_back(data[i]);
        }
        engine::printPearson(vars, parentVars, model);
    }
    //Find the best DAG by k2
    void K2Solver(vstr data, BayesNetworkModel* model){
        if(model == NULL){
            std::cout << "No se ha seleccionado un modelo" << std::endl;
            return;
        }
        if(data.size() < 3){
            std::cout << "Faltan parametros" << std::endl;
            return;
        }
        
        vui vars;
        for(ui i = 0; i < model->variables.size(); i++){//Genrar orden de variables
            vars.push_back(i);
        }
        unsigned int metric = 0;
        if(!data[1].compare("-k2")){
            metric = 0;
        }else if(!data[1].compare("-aic")){
            metric = 1;
        }else if(!data[1].compare("-mdl")){
            metric = 2;
        }
        int numParents = std::stoi(data[2]);
        model->dag = engine::K2(metric, vars, model, numParents);
    }
    //Find the best DAG by brute force
    void BruteForceSolver(vstr data, BayesNetworkModel* model){
        if(model == NULL){
            std::cout << "No se ha seleccionado un modelo" << std::endl;
            return;
        }
        vui vars;
        for(ui i = 0; i < model->variables.size(); i++){
            vars.push_back(i);
        }
        unsigned int i = 0;
        if(data[1].compare("-k2") == 0) i = 0;
        else if(data[1].compare("-aic") == 0) i = 1;
        else if(data[1].compare("-mdl") == 0) i = 2;
        model->dag = engine::BruteForce(i, vars, model);
    }
    //Clear all model attributes
    void ClearSolver(BayesNetworkModel* model){
        if(model == NULL){
            std::cout << "No se ha seleccionado un modelo" << std::endl;
            return;
        }
        model->Clear();
    }
    
    
    
    
    
    //DEV
    void inferenceSolver(vstr data, BayesNetworkModel* model){
        vstr vars;
        for(ui i = 1; i < data.size(); i++){
            vars.push_back(data[i]);
        }
        vui indexes;
        for(ui i = 0; i < vars.size(); i++){
            int temp = model->findVariable(vars[i]);
            if(temp < 0) {
                std::cout << "No se pudo" << std::endl;
                return;
            }
            indexes.push_back((ui)(temp));
        }
    }
}

#endif /* actions_h */
