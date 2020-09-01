//
//  engine.h
//  Topicos Clase 1
//
//  Created by Alonso Diego Esteban Ramirez Sanchez on 8/23/18.
//  Copyright © 2018 Alonso Diego Esteban Ramirez Sanchez. All rights reserved.
//


#ifndef engine_h
#define engine_h

#include <vector>
#include <map>
#include <math.h>
#include <sstream>
#include "Support.h"
#include "BayesNetworkModel.h"
#include "GlobalDefinitions.h"

namespace engine {
    
    namespace core{
        //CORE HELPERS
        ui count(vui vars, vui vals, vvui data){
            ui cont = 0;
            for(ui i = 0; i < data.size(); i++){
                bool flag = true;
                for(ui j = 0; j < vars.size(); j++){
                    if(data[i][vars[j]] != vals[j]){
                        flag = false;
                        break;
                    }
                }
                if(flag) cont++;
            }
            return cont;
        }
        vd normalize(vd probs, vui vars, vui cards){
            ui factSize = 0.0f;
            if(cards.size() > 1){
                factSize = 1.0f;
                for(ui i = 1; i < vars.size(); i++) factSize *= cards[vars[i]];
            }
            for(ui i = 0; i < factSize; i++){
                double max = 0;
                for(ui j = 0; j < cards[vars[0]]; j++){
                    max += probs[i + factSize*j];
                }
                for(ui j = 0; j < cards[vars[0]]; j++){
                    probs[i + factSize*j] /= max;
                }
            }
            return probs;
        }
        ui customCount(vui vars, vui vals, vvui combs, vui counts){
            
            ui c = 0;
            for(ui i = 0; i < combs.size(); i++){
                bool flag = true;
                for(ui j = 0; j < vars.size(); j++){
                    if(combs[i][vars[j]] != vals[j]){
                        flag = false;
                        break;
                    }
                }
                if(flag){
                    c += counts[i];
                }
            }
            return c;
        }
        vvui getAllArcs(vui vars){
            vvui arcs;
            for(ui i = 0; i < vars.size(); i++){
                for(ui j = 0; j < vars.size(); j++){
                    if(i == j) continue;
                    arcs.push_back({i, j});
                }
            }
            return arcs;
        }
        
        
    };
    
    namespace probs{
        
        double MarginalProbability(vui vars, vui vals,  BayesNetworkModel* model){
            double numerator = core::count(vars, vals, model->data) + model->alpha;
            double denominator = model->data.size() + model->alpha*2;
            return numerator/denominator;
        }
        double JointProbability(vui vars, vui vals,  BayesNetworkModel* model){
            double numerator = core::count(vars, vals, model->data) + model->alpha;
            double denominator = model->data.size();
            double acumAlpha = model->alpha;
            for(ui i = 0; i < vars.size(); i++){
                acumAlpha *= model->cardinalities[vars[i]];
            }
            return numerator/(denominator + acumAlpha);
        }
        double ConditionalProbability(vui childVars, vui childVals, vui parentVars, vui parentVals, BayesNetworkModel* model){
            double numerador = JointProbability(support::merge(parentVars, childVars), support::merge(parentVals, childVals), model);
            double denominador = (parentVars.size() > 0 ? JointProbability(parentVars, parentVals, model) : 1);
            return numerador/denominador;
        }
    };
    
    vd Marginalization(vui vars, vui varsValues, vui marginalizedVars, BayesNetworkModel* model){
        vvui marginalizedCombs = support::allValuesCombinations(marginalizedVars, model->cardinalities);
        vd probs;
        for(ui i = 0; i < marginalizedCombs.size(); i++){
            double p = probs::JointProbability(support::merge(vars, marginalizedVars), support::merge(varsValues, marginalizedCombs[i]), model);
            probs.push_back(p);
        }
        return probs;
    }
    
    
    
    double MutualInformation(vui vars, vui parentVars, BayesNetworkModel* model){
        vvui varsCombs = {support::merge({vars[0]}, parentVars), support::merge({vars[1]}, parentVars), support::merge(vars, parentVars)};
        vvd probs;
        ui parentFactorSize = 1.0f;
        for(ui i = 0; i < parentVars.size(); i++) parentFactorSize *= model->cardinalities[parentVars[i]];
        for(ui i = 0; i < varsCombs.size(); i++){
            probs.push_back(vd());
            vvui valsCombs = support::allValuesCombinations(varsCombs[i], model->cardinalities);
            for(ui j = 0; j < valsCombs.size(); j++){
                ui size = varsCombs[i].size() - parentVars.size();
                probs[i].push_back(probs::ConditionalProbability(support::slice(varsCombs[i], 0, size), support::slice(valsCombs[j], 0, size), support::slice(varsCombs[i], size, varsCombs[i].size()), support::slice(valsCombs[j], size, varsCombs[i].size()), model));
            }
            for(ui j = 0; j < parentFactorSize; j++){
                double sum = 0.0f;
                for(ui k = 0; k < valsCombs.size()/parentFactorSize; k++){
                    sum += probs[i][ui(k*parentFactorSize)];
                }
                for(ui k = 0; k < valsCombs.size()/parentFactorSize; k++){
                    probs[i][ui(k*parentFactorSize)] /= sum;
                }
            }
        }
        double total = 0.0f;
        vvui valsCombs = support::allValuesCombinations(varsCombs[2], model->cardinalities);
        for(ui i = 0; i < valsCombs.size(); i++){
            double jointProbXYZ = probs::JointProbability(varsCombs[2], valsCombs[i], model);
            
            double condProbNumXYZ = probs[2][i];
            int index1 = parentFactorSize*(i/(model->cardinalities[vars[1]]*parentFactorSize)) + i%parentFactorSize;
            int index2 = i%(model->cardinalities[vars[1]]*parentFactorSize);
            double condProbDenXZ = probs[0][index1];
            double condProbDenYZ = probs[1][index2];
            double den = condProbDenXZ*condProbDenYZ;
            if(!(den == 0 || condProbNumXYZ == 0)) total += jointProbXYZ*log2(condProbNumXYZ/den);
        }
        return total;
    }
    double Pearson(vui vars, vui parentVars, BayesNetworkModel* model){
        vui tempVars = support::merge(vars, parentVars);
        vvui combs = support::allValuesCombinations(tempVars, model->cardinalities);
        ui factorSize = support::factoreSize(tempVars, model->cardinalities);
        vui counts;
        for(ui i = 0; i < factorSize; i++) counts.push_back(0);
        for(ui i = 0; i < model->data.size(); i++){
            ui fs = factorSize;
            ui index = 0;
            for(ui j = 0; j < tempVars.size(); j++){
                fs /= model->cardinalities[tempVars[j]];
                index += (fs*model->data[i][tempVars[j]]);
            }
            counts[index] += 1;
        }
        ui vart = vars[0];
        ui vare = vars[1];
        vui varsk = parentVars;
        vvui combsK = support::allValuesCombinations(varsk, model->cardinalities);
        double aa = 0.0f;
        for(ui i = 0; i < model->cardinalities[vart]; i++){
            for(ui j = 0; j < model->cardinalities[vare]; j++){
                if(combsK.size() > 0){
                    for(ui k = 0; k < combsK.size(); k++){
                        ui ntek = core::customCount(tempVars, support::merge({i, j}, combsK[k]), combs, counts);
                        ui ntk = core::customCount(support::merge({vart}, varsk), support::merge({i}, combsK[k]), combs, counts);
                        ui nek = core::customCount(support::merge({vare}, varsk), support::merge({j}, combsK[k]), combs, counts);
                        ui nk = core::customCount(varsk, combsK[k], combs, counts);
                        double mtek = (ntk*nek+0.0f)/nk;
                        if(!isnan(mtek) && !isinf(mtek) && mtek != 0){
                            double p = pow(ntek-mtek, 2)/mtek;
                            aa += p;
                        }
                    }
                }else{
                    ui nte = core::customCount({vart, vare}, {i, j}, combs, counts);
                    ui nt = core::customCount({vart}, {i}, combs, counts);
                    ui ne = core::customCount({vare}, {j}, combs, counts);
                    ui n = model->data.size();
                    double mtek = (nt*ne+0.0f)/n;
                    if(!isnan(mtek) && !isinf(mtek) && mtek != 0){
                        double p = pow(nte-mtek, 2)/mtek;
                        aa += p;
                    }
                }
            }
        }
        return aa;
    }
    
    
    namespace QM{
        
        vui findSz(ui k, vui vars, vui parent){
            vui zss;
            for(ui j = 0; j < k; j++){
                zss.push_back(vars[j]);
            }
            for(ui i = 0; i < zss.size(); i++){
                for(ui j = 0; j < parent.size(); j++){
                    if(zss[i] == parent[j]){
                        zss.erase(zss.begin()+i);
                        i -= 1;
                        break;
                    }
                }
            }
            return zss;
        }
        
        double K2Log(ui var, vui parents, BayesNetworkModel* model){
            double score = 0.0f;
            if(parents.size()){
                vvui parentCombs = support::allValuesCombinations(parents, model->cardinalities);
                for(ui i = 0; i < parentCombs.size(); i++){
                    ui cnt = core::count(parents, parentCombs[i], model->data);
                    for(ui j = (model->cardinalities[var] - 1 + cnt); j > (model->cardinalities[var] - 1); j--){
                        score -= log2(j);
                    }
                    for(ui j = 0; j < (model->cardinalities[var]); j++){
                        ui c = core::count(support::merge({var}, parents), support::merge({j}, parentCombs[i]), model->data);
                        for(unsigned int k = 2; k <= c; k++){
                            score += log2(k);
                        }
                    }
                    
                }
            }else{
                for(unsigned int i = (model->data.size() + model->cardinalities[var] - 1); i > (model->cardinalities[var] - 1); i--){
                    
                    //score /= i;
                    
                    score -= log2(i);
                }
                for(unsigned int i = 0; i < model->cardinalities[var]; i++){
                    ui cnt = core::count({var}, {i}, model->data);
                    
                    for(ui j = 2; j <= cnt; j++){
                        score += log2(j);
                    }
                }
            }
            return score;
        }
        double Entropy(ui var, vui parents, BayesNetworkModel* model){
            double score = 0.0f;
            for(unsigned int i = 0; i < model->cardinalities[var]; i++){
                if(parents.size() > 0){
                    vvui parentsValueComb = support::allValuesCombinations(parents, model->cardinalities);
                    for(unsigned int j = 0; j < parentsValueComb.size(); j++){
                        double p = probs::ConditionalProbability({var}, {i}, parents, parentsValueComb[j], model);
                        if(p > 0){
                            score += core::count(support::merge({var}, parents), support::merge({i}, parentsValueComb[j]), model->data)*log2(p);
                        }
                    }
                }else{
                    double p = probs::JointProbability({var}, {i}, model);
                    if(p > 0){
                        score += core::count({var}, {i}, model->data)*log2(p);
                    }
                }
            }
            return score;
        }
        
        double K(ui var, vui parents, BayesNetworkModel* model){
            double qi = 1.0f;
            for(unsigned int i = 0; i < parents.size(); i++){
                qi *= model->cardinalities[parents[i]];
            }
            return (model->cardinalities[var]-1)*qi;
        }
        double AIC(ui var, vui parents, BayesNetworkModel* model){
            return Entropy(var, parents, model) + K(var, parents, model);
        }
        double MDL(ui var, vui parents, BayesNetworkModel* model){
            return Entropy(var, parents, model) + (K(var, parents, model)/2)*log2(model->data.size());
        }
    };
    
    vvb K2(ui metric, vui vars, BayesNetworkModel* model, int numParents){
        vvb dag = support::genMatrix(model->variables.size(), model->variables.size());
        double score = 0.0f;
        for(unsigned int i = 0; i < vars.size(); i++){
            vui parents;
            double Pold = 0;
            switch (metric) {
                case 0:
                    Pold = QM::K2Log(vars[i], {}, model);
                    break;
                case 1:
                    Pold = QM::AIC(vars[i], {}, model);
                    break;
                case 2:
                    Pold = QM::MDL(vars[i], {}, model);
                    break;
            }
            bool OKToProceed = true;
            while(OKToProceed && parents.size() < i && parents.size() < numParents){
                int z = -1;
                double Pnew = (metric == 0 ? -2147483648 : 2147483647);
                vui zS = QM::findSz(i, vars, parents);
                for(unsigned int j = 0; j < zS.size(); j++){
                    if(support::find(parents, zS[j]) > -1) continue;
                    double t = 0;//K2Log(vars[i], support::merge(parents, {zS[j]}), model);
                    bool flag;
                    switch (metric) {
                        case 0:
                            t = QM::K2Log(vars[i], support::merge(parents, {zS[j]}), model);
                            flag = (t > Pnew);
                            break;
                        case 1:
                            t = QM::AIC(vars[i], support::merge(parents, {zS[j]}), model);
                            flag = (t < Pnew);
                            break;
                        case 2:
                            t = QM::MDL(vars[i], support::merge(parents, {zS[j]}), model);
                            flag = (t < Pnew);
                            break;
                    }
                    if(flag){
                        z = zS[j];
                        Pnew = t;
                    }
                }
                bool flag;
                switch (metric) {
                    case 0:
                        flag = (Pnew > Pold);
                        break;
                    case 1:
                        flag = (Pnew < Pold);
                        break;
                    case 2:
                        flag = (Pnew < Pold);
                        break;
                }
                if(flag){
                    Pold = Pnew;
                    parents.push_back(z);
                }else{
                    OKToProceed = false;
                }
            }
            score += Pold;
            for(unsigned int j = 0; j < parents.size(); j++){
                dag[vars[i]][parents[j]] = 1;
            }
        }
        for(unsigned int j = 0; j < dag.size(); j++){
            support::print(dag[j]);
            vui parents = support::findParents(dag[j]);
            support::nextLine();
        }
        std::cout << "Puntaje: " << score << std::endl;
        
        FileManager instance = FileManager();
        std::string fileName = model->name + "K2";
        instance.createFile(fileName + ".csv");
        instance.fileOpen(fileName + ".csv");
        instance.writeLine("Matriz:\n");
        instance.writeMatrix(dag);
        instance.writeLine("Puntaje: " + std::to_string(score));
        instance.fileClose();
        
        return dag;
    }
    
    vvb BruteForce(ui metric, vui vars, BayesNetworkModel* model){
        
        
        std::vector<std::vector<unsigned int>> arcs;
        for(unsigned int i = 0; i < vars.size(); i++){
            for(unsigned int j = 0; j < vars.size(); j++){
                if(i == j) continue;
                arcs.push_back({vars[i], vars[j]});
            }
        }
        
        FileManager instanceDAGs;
        FileManager instanceScores;
        
        instanceDAGs = FileManager();
        instanceScores = FileManager();
        instanceDAGs.createFile(model->name + "_BruteForce_DAGs.csv");
        instanceDAGs.fileOpen(model->name + "_BruteForce_DAGs.csv");
        instanceScores.createFile(model->name + "_BruteForce_Scores.csv");
        instanceScores.fileOpen(model->name + "_BruteForce_Scores.csv");
        switch (metric) {
            case 0:
                instanceScores.writeLine("index,K2\n");
                break;
            case 1:
                instanceScores.writeLine("index,AIC\n");
                break;
            case 2:
                instanceScores.writeLine("index,MDL\n");
                break;
        }
        
        double bestScore = 0;
        double worstScore = 0;
        if(metric == 0){
            worstScore = 2147483647;
            bestScore = -2147483647;
        }else{
            bestScore = 2147483647;
            worstScore = -2147483647;
        }
        unsigned int bestIndex = 0;
        
        ResponseGroup<std::vector<std::vector<bool>>> response;
        long long dagCount = 0;
        long long maxSize = pow(2, arcs.size());
        for(long long i = 0; i < maxSize; i++){
            long long index = i;
            std::vector<std::vector<bool>> dag = support::genMatrix(model->variables.size(), model->variables.size());
            for(int j = arcs.size() - 1; j >= 0; j--){
                unsigned int factor = pow(2, j);
                if(index >= factor){
                    dag[arcs[j][1]][arcs[j][0]] = 1;
                    index -= factor;
                }
            }
            if(support::isDAG(dag)){
                response.res.push_back(dag);
                
                std::cout << i << std::endl;
                for(ui j = 0; j < dag.size(); j++){
                    support::print(dag[j]);
                    support::nextLine();
                }
                double currentScore = 0.0f;
                for(unsigned int j = 0; j < dag.size(); j++){
                    switch (metric) {
                        case 0:
                            currentScore += QM::K2Log(j, support::findParents(dag[j]), model);
                            break;
                        case 1:
                            currentScore += QM::AIC(j, support::findParents(dag[j]), model);
                            break;
                        case 2:
                            currentScore += QM::MDL(j, support::findParents(dag[j]), model);
                            break;
                    }
                }
                instanceScores.writeLine(std::to_string(dagCount) + "," + std::to_string(currentScore) + "\n");
                instanceDAGs.writeLine(std::to_string(dagCount) + "\n");
                instanceDAGs.writeMatrix(dag);
                response.scores.push_back(currentScore);
                bool flag;
                switch (metric) {
                    case 0:
                        flag = (currentScore > bestScore);
                        std::cout << "K2: ";
                        break;
                    case 1:
                        flag = (currentScore < bestScore);
                        std::cout << "AIC: ";
                        break;
                    case 2:
                        flag = (currentScore < bestScore);
                        std::cout << "MDL: ";
                        break;
                }
                std::cout << currentScore <<std::endl;
                if(flag){
                    bestIndex = dagCount;
                    bestScore = currentScore;
                }
                if(metric == 0 && currentScore < worstScore) worstScore = currentScore;
                if(metric > 0 && currentScore > worstScore) worstScore = currentScore;
                //instanceDAGs.writeLine("Indice: " + std::to_string(i) + "\n");
                //instanceDAGs.writeMatrix(dag);
                //instanceScores.writeLine(std::to_string(i) + "," + std::to_string(currentK2) + "," + std::to_string(currentAIC) + "," + std::to_string(currentMDL) + "\n");
                dagCount += 1;
            }
        }
        
        instanceDAGs.fileClose();
        instanceScores.fileClose();
        
        std::cout << "Los mejores grafos por Metrica de calidad" <<std::endl;
        for(int i = 0; i < response.res[bestIndex].size(); i++){
            support::print(response.res[bestIndex][i]);
            support::nextLine();
        }
        std::cout << "Indice: " << bestIndex << " Metrica K2: " << bestScore <<std::endl;
        std::cout << "Peor puntaje: " << worstScore <<std::endl;
        return response.res[bestIndex];
    }
    
    //PRINT
    void printMarginalProbabilityWithValue(str var, str val, BayesNetworkModel* model){
        int indexVar = model->findVariable(var);
        if(indexVar < 0){
            std::cout << "Variable no existe" << std::endl;
            return;
        }
        int indexVal = model->findValue(var, val);
        if(indexVal < 0){
            std::cout << "Valor no existe no existe" << std::endl;
            return;
        }
        std::cout << "P(" << model->variables[indexVar] << " = " << model->values[indexVar][indexVal] << ") = " << probs::MarginalProbability({ui(indexVar)}, {ui(indexVal)}, model) << std::endl;
    }
    void printMarginalProbability(str var, BayesNetworkModel* model){
        int index = model->findVariable(var);
        if(index < 0){
            std::cout << "Variable no existe" << std::endl;
            return;
        }
        
        double p = 0.0f;
        for(ui i = 0; i < model->cardinalities[index]; i++){
            double prob = probs::MarginalProbability({ui(index)}, {i}, model);
            p += prob;
            std::cout << "P(" << model->variables[index] << " = " << model->values[index][i] << ") = " << prob << std::endl;
        }
        std::cout << "Distribución: " << p << std::endl;
    }
    void printJointProbability(vstr vars, BayesNetworkModel* model){
        vui varsIndexes;
        for(ui i = 0; i < vars.size(); i++){
            int index = model->findVariable(vars[i]);
            if(index < 0){
                std::cout << "Variables no existe" << std::endl;
                return;
            }
            varsIndexes.push_back(index);
        }
        support::print(vars);
        support::nextLine();
        vvui combs = support::allValuesCombinations(varsIndexes, model->cardinalities);
        double p = 0.0f;
        for(ui i = 0; i < combs.size(); i++){
            for(ui j = 0; j < combs[i].size(); j++){
                std::cout << model->values[varsIndexes[j]][combs[i][j]] << " ";
            }
            double prob = probs::JointProbability(varsIndexes, combs[i], model);
            p += prob;
            std::cout << "-> " << prob << std::endl;
        }
        std::cout << "Distribución: " << p << std::endl;
    }
    void printConditionalProbability(vstr vars, vstr parentVars, BayesNetworkModel* model){
        vui varsIndexes;
        for(ui i = 0; i < vars.size(); i++){
            int index = model->findVariable(vars[i]);
            if(index < 0){
                std::cout << "Variables no existe" << std::endl;
                return;
            }
            varsIndexes.push_back(index);
        }
        vui parentVarsIndexes;
        for(ui i = 0; i < parentVars.size(); i++){
            int index = model->findVariable(parentVars[i]);
            if(index < 0){
                std::cout << "Variables no existe" << std::endl;
                return;
            }
            parentVarsIndexes.push_back(index);
        }
        vvui parentValsCombs = support::allValuesCombinations(parentVarsIndexes, model->cardinalities);
        vvui valsCombs = support::allValuesCombinations(varsIndexes, model->cardinalities);
        for(ui i = 0; i < parentValsCombs.size(); i++){
            support::print(vars);
            std::cout << "| ";
            support::print(parentVars);
            support::nextLine();
            double max = 0.0f;
            vd probs;
            for(ui j = 0; j < valsCombs.size(); j++){
                double p = probs::ConditionalProbability(varsIndexes, valsCombs[j], parentVarsIndexes, parentValsCombs[i], model);
                probs.push_back(p);
                max += p;
            }
            for(ui j = 0; j < probs.size(); j++){
                probs[j] /= max;
                
                for(ui k = 0; k < valsCombs[j].size(); k++){
                    std::cout << model->values[varsIndexes[k]][valsCombs[j][k]] << " ";
                }
                for(ui k = 0; k < parentValsCombs[i].size(); k++){
                    std::cout << model->values[parentVarsIndexes[k]][parentValsCombs[i][k]] << " ";
                }
                std::cout << "-> " << probs[j] << std::endl;
            }
            std::cout << "Distribucion: " << support::sum(probs) << std::endl;
        }
    }
    void printMarginalization(vstr vars, vstr marginalizedVars, BayesNetworkModel* model){
        vui varsIndexes;
        for(ui i = 0; i < vars.size(); i++){
            int index = model->findVariable(vars[i]);
            if(index < 0){
                std::cout << "Variables no existe" << std::endl;
                return;
            }
            varsIndexes.push_back(index);
        }
        vui marginalizedVarsIndexes;
        for(ui j = 0; j < marginalizedVars.size(); j++){
            int index = model->findVariable(marginalizedVars[j]);
            if(index < 0){
                std::cout << "Variable no existe" << std::endl;
                return;
            }
            marginalizedVarsIndexes.push_back(index);
        }
        vvui combs = support::allValuesCombinations(varsIndexes, model->cardinalities);
        for(ui i = 0; i < combs.size(); i++){
            support::print(vars);
            support::print(marginalizedVars);
            support::nextLine();
            vd probs = Marginalization(varsIndexes, combs[i], marginalizedVarsIndexes, model);
            
            vvui marginalizedCombs = support::allValuesCombinations(marginalizedVarsIndexes, model->cardinalities);
            for(ui j = 0; j < marginalizedCombs.size(); j++){
                for(ui k = 0; k < combs[i].size(); k++){
                    std::cout << model->values[varsIndexes[k]][combs[i][k]] << " ";
                }
                for(ui k = 0; k < marginalizedCombs[j].size(); k++){
                    std::cout << model->values[marginalizedVarsIndexes[k]][marginalizedCombs[j][k]] << " ";
                }
                std::cout << probs[j] << std::endl;
            }
            std::cout << "Marginalizacion de ";
            support::print(marginalizedVars);
            std::cout << "para ";
            for(ui j = 0; j < vars.size(); j++){
                std::cout << vars[j] << " = " << combs[i][j] << " ";
            }
            std::cout << "Resultado: " << support::sum(probs);
            support::nextLine();
        }
    }
    void printMutualInformation(vstr vars, vstr parentVars, BayesNetworkModel* model){
        vui varsIndexes;
        for(ui i = 0; i < vars.size(); i++){
            int index = model->findVariable(vars[i]);
            if(index < 0){
                std::cout << "Variables no existe" << std::endl;
                return;
            }
            varsIndexes.push_back(index);
        }
        vui parentVarsIndexes;
        for(ui j = 0; j < parentVars.size(); j++){
            int index = model->findVariable(parentVars[j]);
            if(index < 0){
                std::cout << "Variable no existe" << std::endl;
                return;
            }
            parentVarsIndexes.push_back(index);
        }
        std::cout << "MI( ";
        support::print(vars);
        std::cout << "| ";
        support::print(parentVars);
        std::cout << ")";
        support::nextLine();
        double mi = MutualInformation(varsIndexes, parentVarsIndexes, model);
        std::cout << "Grado de independencia Mutual Information: " << mi << std::endl;
    }
    void printPearson(vstr vars, vstr parentVars, BayesNetworkModel* model){
        vui varsIndexes;
        for(ui i = 0; i < vars.size(); i++){
            int index = model->findVariable(vars[i]);
            if(index < 0){
                std::cout << "Variables no existe" << std::endl;
                return;
            }
            varsIndexes.push_back(index);
        }
        vui parentVarsIndexes;
        for(ui j = 0; j < parentVars.size(); j++){
            int index = model->findVariable(parentVars[j]);
            if(index < 0){
                std::cout << "Variable no existe" << std::endl;
                return;
            }
            parentVarsIndexes.push_back(index);
        }
        std::cout << "Grado de independencia Pearson: " << Pearson(varsIndexes, parentVarsIndexes, model) << std::endl;
    }
    bool _isCicle(std::vector<Triplete<unsigned int, unsigned int, double>> edges, unsigned int actualNode, std::vector<bool> visitedNodes){
        if(visitedNodes[actualNode]) return true;
        visitedNodes[actualNode] = true;
        for(unsigned int i = 0; i < edges.size(); i++){
            int t = 0;
            if(edges[i].first == actualNode) t = 1;
            if(edges[i].second == actualNode) t = 2;
            if(t > 0){
                unsigned int nextNode;
                switch (t) {
                    case 1:
                        if(visitedNodes[edges[i].second]) return true;
                        nextNode = edges[i].second;
                        break;
                    case 2:
                        if(visitedNodes[edges[i].first]) return true;
                        nextNode = edges[i].first;
                        break;
                    default:
                        break;
                }
                edges.erase(edges.begin() + i);
                if(_isCicle(edges, nextNode, visitedNodes)) return true;
                i -= 1;
            }
        }
        return false;
    }
    
    bool isCicle(std::vector<Triplete<unsigned int, unsigned int, double>> edges, unsigned int numVars){
        std::vector<bool> visitedNodes(numVars);
        for(unsigned int i = 0; i < numVars; i++){
            if(_isCicle(edges, i, visitedNodes)){
                return true;
            }
        }
        return false;
    }
    
    std::vector<std::vector<bool>> Kruskal(std::vector<Triplete<unsigned int, unsigned int, double>> edges, unsigned int numVars){
        for(unsigned int i = 0; i < edges.size() - 1; i++){
            for(unsigned int j = i + 1; j < edges.size(); j++){
                if(edges[i].third < edges[j].third){
                    Triplete<unsigned int, unsigned int, double> aux = edges[i];
                    edges[i] = edges[j];
                    edges[j] = aux;
                }
            }
        }
        std::vector<Triplete<unsigned int, unsigned int, double>> graph;
        while(graph.size() < (numVars-1) && edges.size() > 0){
            graph.push_back(edges[0]);
            edges.erase(edges.begin());
            if(isCicle(graph, numVars)) graph.erase(graph.end()-1);
        }
        std::vector<std::vector<bool>> dag = support::genMatrix(numVars, numVars);
        std::vector<unsigned int> usedNodes = {0};
        while(usedNodes.size() > 0){
            for(unsigned int i = 0; i < graph.size(); i++){
                int t = 0;
                if(graph[i].first == usedNodes[0]) t = 1;
                if(graph[i].second == usedNodes[0]) t = 2;
                if(t > 0){
                    switch (t) {
                        case 1:
                            dag[graph[i].second][usedNodes[0]] = 1;
                            if(support::find(usedNodes, graph[i].second) < 0) usedNodes.push_back(graph[i].second);
                            break;
                        case 2:
                            dag[graph[i].first][usedNodes[0]] = 1;
                            if(support::find(usedNodes, graph[i].first) < 0) usedNodes.push_back(graph[i].first);
                            break;
                            
                        default:
                            break;
                    }
                    graph.erase(graph.begin() + i);
                    i -= 1;
                }
            }
            usedNodes.erase(usedNodes.begin());
        }
        return  dag;
    }
    double I(unsigned int xi, unsigned int xj, BayesNetworkModel* model){
        double res = 0.0f;
        for(unsigned int i = 0; i < model->cardinalities[xi]; i++){
            for(unsigned int j = 0; j < model->cardinalities[xj]; j++){
                double joint = probs::JointProbability({xi, xj}, {i, j}, model);
                double d = joint*log2(joint/(probs::JointProbability({xi}, {i}, model)*probs::JointProbability({xj}, {j}, model)));
                if(isnan(d) || isinf(d)) continue;
                res += d;
            }
        }
        return res;
    }
    std::vector<std::vector<bool>> ChowLiu(BayesNetworkModel* model){
        std::vector<Triplete<unsigned int, unsigned int, double>> edges;
        for(unsigned int i = 0; i < model->variables.size(); i++){
            for(unsigned int j = 0; j < model->variables.size(); j++){
                if(i >= j) continue;
                Triplete<unsigned int, unsigned int, double> tri;
                tri.first = i;
                tri.second = j;
                tri.third = I(i, j, model);
                edges.push_back(tri);
                std::cout << model->variables[tri.first] << "<->" << model->variables[tri.second] << " Puntaje: " << tri.third << std::endl;
            }
        }
        std::vector<std::vector<bool>> graph = Kruskal(edges, model->variables.size());
        for(unsigned int i = 0; i < graph.size(); i++){
            support::print(graph[i]);
            support::nextLine();
        }
        return graph;
    }
    
    std::vector<double> getDistribution(Factor factor, BayesNetworkModel* model){
        std::vector<double> dist = model->mappedValues[factor.vars[0]];
        std::vector<std::vector<unsigned int>> valComb = support::allValuesCombinations(factor.vars,model->cardinalities);
        for(int i = 0; i < dist.size(); i++){
            for(unsigned int j = 0; j < valComb[i].size(); j++){
                if(factor.vals[j] > -1 && factor.vals[j] != valComb[i][j]){
                    dist.erase(dist.begin() + i);
                    valComb.erase(valComb.begin() + i);
                    i -= 1;
                    break;
                }   
            }
        }
        return dist;
    }
    
    int getIndex(Factor factor , std::vector<unsigned int> valsOrder, std::vector<unsigned int> varsCorrectOrder, std::vector<unsigned int> cardinalities){
        std::vector<unsigned int> varsOrder = factor.vars;
        std::vector<unsigned int> vals(varsCorrectOrder);
        for(unsigned int i = 0; i < varsOrder.size(); i++){
            int subIndex = support::find(varsCorrectOrder, varsOrder[i]);
            if(subIndex > -1){
                vals[subIndex] = valsOrder[i];
            }
        }
        
        unsigned int factorSize = 1.0f;
        for(unsigned int i = 0; i < varsCorrectOrder.size(); i++){
            if(factor.vals[support::find(factor.vars, varsCorrectOrder[i])] != -1) continue;
            factorSize *= cardinalities[varsCorrectOrder[i]];
        }
        int index = 0;
        for(unsigned int i = 0; i < varsCorrectOrder.size(); i++){
            if(factor.vals[support::find(factor.vars, varsCorrectOrder[i])] != -1) continue;
            factorSize /= cardinalities[varsCorrectOrder[i]];
            index += (vals[i]*factorSize);
        }
        return index;
    }
    
    Factor mergeFactor(std::vector<Factor> toMergeFactors, unsigned int var, BayesNetworkModel* model){
        while (toMergeFactors.size() > 1) {
            Factor factor;
            factor.vars = toMergeFactors[0].vars;
            factor.vals = toMergeFactors[0].vals;
            for(unsigned int i = 0; i < toMergeFactors[1].vars.size(); i++){
                if(support::find(factor.vars, toMergeFactors[1].vars[i]) == -1){
                    factor.vars.push_back(toMergeFactors[1].vars[i]);
                    factor.vals.push_back(toMergeFactors[1].vals[i]);
                }
            }
            std::vector<std::vector<unsigned int>> valuesCombination = support::allValuesCombinations(factor.vars, model->cardinalities);
            for(unsigned int i = 0; i < valuesCombination.size(); i++){
                bool flag = true;
                for(unsigned int j = 0; j < factor.vars.size(); j++){
                    if(factor.vals[j] > -1 && valuesCombination[i][j] != factor.vals[j]){
                        flag = false;
                        break;
                    }
                }
                if(flag){
                    int index1 = getIndex(factor, valuesCombination[i], toMergeFactors[0].vars, model->cardinalities);
                    int index2 = getIndex(factor, valuesCombination[i], toMergeFactors[1].vars, model->cardinalities);
                    //std::cout << index1 << " " << index2 << " " << toMergeFactors[0].probs[index1] << " " << toMergeFactors[1].probs[index2] << " "<< toMergeFactors[0].probs[index1]*toMergeFactors[1].probs[index2] << " "  << std::endl;
                    factor.probs.push_back(toMergeFactors[0].probs[index1]*toMergeFactors[1].probs[index2]);
                }
                
            }
            toMergeFactors.erase(toMergeFactors.begin());
            toMergeFactors.erase(toMergeFactors.begin());
            toMergeFactors.push_back(factor);
        }
        unsigned int factorSize = 1.0f;
        unsigned int offset = 1.0f;
        unsigned int index = support::find(toMergeFactors[0].vars, var);
        for(unsigned i = index; i < toMergeFactors[0].vars.size(); i++){
            if(toMergeFactors[0].vals[i] != -1) continue;
            offset *= model->cardinalities[toMergeFactors[0].vars[i]];
        }
        factorSize = offset/model->cardinalities[toMergeFactors[0].vars[index]];
        toMergeFactors[0].vars.erase(toMergeFactors[0].vars.begin() + index);
        if(toMergeFactors[0].vals[index] > -1){
            toMergeFactors[0].vals.erase(toMergeFactors[0].vals.begin() + index);
            return toMergeFactors[0];
        }
        toMergeFactors[0].vals.erase(toMergeFactors[0].vals.begin() + index);
        std::vector<double> probs;
        for(unsigned i = 0; i < (toMergeFactors[0].probs.size()/model->cardinalities[var]); i++){
            double p = 0.0f;
            unsigned int subIndex = ((offset*(i/factorSize)+i%factorSize));
            for(unsigned j = 0; j < model->cardinalities[var]; j++){
                p += toMergeFactors[0].probs[subIndex+j*factorSize];
            }
            probs.push_back(p);
        }
        toMergeFactors[0].probs = probs;
        return toMergeFactors[0];
    }
    
    std::vector<unsigned int> getMarginalizeOrder(std::vector<unsigned int> vars, BayesNetworkModel* model){
        if(vars.size() == 0) return vars;
        std::vector<unsigned int> numParents;
        for(int i = 0; i < vars.size(); i++){
            numParents.push_back(support::findParents(model->dag[vars[i]]).size());
        }
        for(int i = 0; i < numParents.size() - 1; i++){
            for(int j = i + 1; j < numParents.size(); j++){
                if(numParents[i] > numParents[j]){
                    unsigned int temp = numParents[i];
                    numParents[i] = numParents[j];
                    numParents[j] = temp;
                    temp = vars[i];
                    vars[i] = vars[j];
                    vars[j] = temp;
                }
            }
        }
        return vars;
    }
    
    void variableElimination(std::vector<unsigned int> vars, std::vector<int> vals, BayesNetworkModel* model){
        
        std::vector<Factor> factors;
        std::vector<bool> variableVisited(model->variables.size());
        std::vector<unsigned int> variableToVisit = vars;
        std::vector<unsigned int> variableToMarginalized;
        while (variableToVisit.size() > 0) {
            unsigned int current = variableToVisit[0];
            if(variableVisited[current]){
                variableToVisit.erase(variableToVisit.begin());
                continue;
            }
            Factor factor;
            std::vector<unsigned int> parents = support::findParents(model->dag[current]);
            factor.vars = support::merge({current}, parents);
            for(unsigned int i = 0; i < factor.vars.size(); i++){
                int index = support::find(vars, factor.vars[i]);
                if(vals.size() > 0 && index > -1) factor.vals.push_back(vals[index]);
                else factor.vals.push_back(-1);
                if(i > 0)
                    if(support::find(vars, parents[i-1]) < 0 && support::find(variableToMarginalized, parents[i-1]) < 0)
                        variableToMarginalized.push_back(parents[i-1]);
            }
            factor.probs = getDistribution(factor, model);
            variableToVisit = support::merge(variableToVisit, parents);
            factors.push_back(factor);
            variableVisited[variableToVisit[0]] = true;
            variableToVisit.erase(variableToVisit.begin());
        }
        
        
        
        for(unsigned int i = 0; i < factors.size(); i++){
            std::cout << "P(" << model->variables[factors[i].vars[0]];
            if(factors[i].vals[0] > -1){
                std::cout << "=" << model->values[factors[i].vars[0]][factors[i].vals[0]];
            }
            if(factors[i].vars.size() == 1){
                std::cout << ")";
                continue;
            }
            std::cout << "|";
            for(unsigned int j = 1; j < factors[i].vars.size(); j++){
                std::cout << model->variables[factors[i].vars[j]];
                if(factors[i].vals[j] > -1){
                    std::cout << "=" << model->values[factors[i].vars[j]][factors[i].vals[j]];
                }
                std::cout << ",";
            }
            std::cout << ")";
        }
        support::nextLine();
        
        
        double constantFactors = 1.0f;
        for(unsigned int i = 0; i < factors.size(); i++){
            if(factors[i].probs.size() == 1){
                constantFactors *= factors[i].probs[0];
                factors.erase(factors.begin() + i);
                i -= 1;
            }
        }
        variableToMarginalized = getMarginalizeOrder(variableToMarginalized, model);
        
        while(variableToMarginalized.size() > 0){
            unsigned int currentVar = variableToMarginalized[0];
            std::cout << "Marginalizando: " << model->variables[currentVar] << std::endl;
            variableToMarginalized.erase(variableToMarginalized.begin());
            std::vector<Factor> toMergeFactors;
            for(unsigned int i = 0; i < factors.size(); i++){
                if(support::find(factors[i].vars, currentVar) > -1){
                    toMergeFactors.push_back(factors[i]);
                    factors.erase(factors.begin() + i);
                    i -= 1;
                }
            }
            factors.push_back(mergeFactor(toMergeFactors, currentVar, model));
        }
        
        for(unsigned i = 0; i < factors.size() && factors.size() > 1; i++){
            if(support::find(factors[i].vars, vars[0]) > -1){
                Factor temp = factors[0];
                factors[0] = factors[i];
                factors[i] = temp;
            }
        }
        while(factors.size() > 1){
            if(factors[1].probs.size() == 1){
                for(unsigned int i = 0; i < factors[0].probs.size(); i++){
                    factors[0].probs[i] *= factors[1].probs[0];
                }
                factors.erase(factors.begin() + 1);
                continue;
            }
            if(factors[0].probs.size() != factors[1].probs.size()){
                std::cout << "Hubo un error, el tamanio de los factores finales es diferentes" << std::endl;
                return;
            }
            for(unsigned int i = 0; i < factors[0].probs.size(); i++){
                factors[0].probs[i] *= factors[1].probs[i];
            }
            factors.erase(factors.begin() + 1);
        }
        std::cout << "Las probabilidades son:" << std::endl;
        double max = 0.0f;
        for(unsigned int i = 0; i < factors[0].probs.size(); i++){
            factors[0].probs[i] = factors[0].probs[i]*constantFactors;
            max += factors[0].probs[i];
        }
        for(unsigned int i = 0; i < factors[0].probs.size(); i++){
            factors[0].probs[i] /= max;
            std::cout << "Para " << model->variables[vars[0]] << " = " << model->values[vars[0]][i] << " -> " << factors[0].probs[i] << std::endl;
        }
    }
    
    void MaxLikelihood(std::vector<unsigned int> vars, std::vector<int> vals, BayesNetworkModel* model){
        std::vector<double> dist;
        for(unsigned int i = 0; i < model->cardinalities[vars[0]]; i++){
            dist.push_back(probs::JointProbability({vars[0]}, {i}, model));
        }
        if(vars.size() > 1){
            std::vector<unsigned int> parentVals;
            for(unsigned int i = 1; i < vals.size(); i++){
                parentVals.push_back(i);
            }
            double max = 0.0f;
            double evidenceFactor = probs::JointProbability(support::slice(vars, 1, vars.size()), parentVals, model);
            for(unsigned int i = 0; i < model->cardinalities[vars[0]]; i++){
                dist[i] /= evidenceFactor;
                max += dist[i];
            }
            for(unsigned int i = 0; i < model->cardinalities[vars[0]]; i++){
                dist[i] /= max;
            }
        }
        for(unsigned int i = 0; i < model->cardinalities[vars[0]]; i++){
            std::cout << "Para " << model->variables[vars[0]] << " = " << model->values[vars[0]][i] << " -> " << dist[i] << std::endl;
        }
    }
    
    void fit(BayesNetworkModel* model){
        for(unsigned int i = 0; i < model->variables.size(); i++){
            model->mappedValues.push_back(std::vector<double>());
            std::vector<unsigned int> parents = support::findParents(model->dag[i]);
            std::vector<std::vector<unsigned int>> parentValues = support::allValuesCombinations(parents, model->cardinalities);
            if(parents.size() == 0){
                for(unsigned int j = 0; j < model->cardinalities[i]; j++){
                    model->mappedValues[i].push_back(probs::JointProbability({i}, {j}, model));
                }
            }else{
                for(unsigned int j = 0; j < model->cardinalities[i]; j++){
                    for(unsigned int k = 0; k < parentValues.size(); k++){
                        model->mappedValues[i].push_back(probs::ConditionalProbability({i}, {j}, parents, parentValues[k], model));
                    }
                }
            }
            if(model->alpha > 0 && parents.size() > 0){
                unsigned int factorSize = 1.0f;
                for(unsigned int j = 0; j < parents.size(); j++){
                    factorSize *= model->cardinalities[parents[j]];
                }
                for(unsigned int j = 0; j < parentValues.size(); j++){
                    double max = 0.0f;
                    for(unsigned int k = 0; k < model->cardinalities[i]; k++){
                        max += model->mappedValues[i][factorSize*k+j];
                    }
                    for(unsigned int k = 0; k < model->cardinalities[i]; k++){
                        model->mappedValues[i][factorSize*k+j] /= max;
                    }
                }
            }
            std::cout << "P(" << model->variables[i];
            if(parents.size() > 0){
                std::cout << "|";
                for(unsigned int j = 0; j < parents.size(); j++){
                    std::cout << model->variables[parents[j]];
                    if((j+1) < parents.size()) std::cout << ",";
                }
            }
            std::cout << ")" << std::endl;
            if(parents.size() == 0){
                unsigned int offset = 0;
                for(unsigned int j = 0; j < model->values[i].size(); j++){
                    if(offset < model->values[i][j].size()) offset = model->values[i][j].size();
                }
                for(unsigned int j = 0; j < model->values[i].size(); j++){
                    std::cout << model->values[i][j];
                    for(unsigned int k = 0; k < offset - model->values[i][j].size(); k++){
                        std::cout << " ";
                    }
                    std::cout << " -> " << model->mappedValues[i][j] << std::endl;
                }
            }else{
                for(unsigned int j = 0; j < model->cardinalities[i]; j++){
                    unsigned int offset = 0;
                    for(unsigned int k = 0; k < parentValues.size(); k++){
                        unsigned int tempOffset = model->values[i][j].size();
                        for(unsigned int z = 0; z < parentValues[k].size(); z++){
                            tempOffset += model->values[parents[z]][parentValues[k][z]].size();
                        }
                        if(tempOffset > offset) offset = tempOffset;
                    }
                    for(unsigned int k = 0; k < parentValues.size(); k++){
                        std::cout << model->values[i][j] << " ";
                        unsigned int tempOffset = offset;
                        for(unsigned int z = 0; z < parentValues[k].size(); z++){
                            tempOffset -= model->values[parents[z]][parentValues[k][z]].size();
                            std::cout << model->values[parents[z]][parentValues[k][z]] << " ";
                        }
                        for(unsigned int z = 0; z < (tempOffset-model->values[i][j].size()); z++){
                            std::cout << " ";
                        }
                        std::cout << " -> " << model->mappedValues[i][model->cardinalities[i]*j+k] << std::endl;
                    }
                }
            }
        }
    }
    
}

#endif /* engine_h */

