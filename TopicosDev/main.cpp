//
//  main.cpp
//  Topicos Clase 1
//
//  Created by Alonso Diego Esteban Ramirez Sanchez on 8/14/18.
//  Copyright © 2018 Alonso Diego Esteban Ramirez Sanchez. All rights reserved.
//

#include <iostream>
#include <vector>
#include "BayesNetworkModel.h"
#include "Engine.h"
#include "Parser.h"
#include "ActionSolver.h"
#include "MarkovChain.h"
#include "GlobalDefinitions.h"

#include <time.h>

std::vector<BayesNetworkModel*> models;
BayesNetworkModel* current;

void ActionSolver(std::vector<std::string> data){
    if(!data.size()) return;
    if(!data[0].compare("salir")) actionsolver::ExitSolver();
    if(!data[0].compare("nuevo")){
        actionsolver::CreateModelSolver(data, models);
        return;
    }
    if(!data[0].compare("seleccionar")){
        actionsolver::SelectSolver(data, models, current);
        return;
    }//NO FUNCA POR QUE NO SE INICIALIZA CURRENT
    
    if(!data[0].compare("data")){
        actionsolver::DataSolver(data, current);
        return;
    }
    if(!data[0].compare("vars")){
        actionsolver::VarsSolver(data, current);
        return;
    }
    if(!data[0].compare("vals")){
        actionsolver::ValsSolver(data, current);
        return;
    }
    if(!data[0].compare("cards")){
        actionsolver::CardsSolver(current);
        return;
    }
    if(!data[0].compare("arco")){
        actionsolver::ArcSolver(data, current);
        return;
    }
    if(!data[0].compare("dag")){
        actionsolver::DagSolver(current);
        return;
    }
    if(!data[0].compare("alpha")){
        actionsolver::AlphaSolver(data, current);
        return;
    }
    if(!data[0].compare("marginal")){
        actionsolver::MarginalProbSolver(data, current);
        return;
    }
    if(!data[0].compare("conjunta")){
        actionsolver::JointProbSolver(data, current);
        return;
    }
    if(!data[0].compare("condicional")){
        actionsolver::CondProbSolver(data, current);
        return;
    }
    if(!data[0].compare("marginalizacion")){
        actionsolver::MarginalizationSolver(data, current);
        return;
    }
    if(!data[0].compare("mut")){
        actionsolver::MutualInformationSolver(data, current);
        return;
    }
    if(!data[0].compare("pearson")){
        actionsolver::PearsonSolver(data, current);
        return;
    }
    if(!data[0].compare("k2")){
        actionsolver::K2Solver(data, current);
        return;
    }
    if(!data[0].compare("fuerzabruta")){
        actionsolver::BruteForceSolver(data, current);
        return;
    }
    if(!data[0].compare("clear")){
        actionsolver::ClearSolver(current);
        return;
    }
    if(!data[0].compare("demo1")){
        ActionSolver(support::splitLine("nuevo DemoModel1", " "));
        ActionSolver(support::splitLine("seleccionar DemoModel1", " "));
        ActionSolver(support::splitLine("vars fumador alimentacion antecedentes cancer", " "));//4
        ActionSolver(support::splitLine("vals fumador poco medio alto", " "));//3
        ActionSolver(support::splitLine("vals alimentacion malo medio alto", " "));//3
        ActionSolver(support::splitLine("vals antecedentes si no", " "));//2
        ActionSolver(support::splitLine("vals cancer si no", " "));//2
        
        ActionSolver(support::splitLine("data ./DemoData.csv -h", " "));
        
        ActionSolver(support::splitLine("alpha 1", " "));
        
        ActionSolver(support::splitLine("fuerzabruta -k2", " "));
        ActionSolver(support::splitLine("k2 -k2 2", " "));
        ActionSolver(support::splitLine("k2 -aic 2", " "));
        ActionSolver(support::splitLine("k2 -mdl 2", " "));
        return;
    }
    if(!data[0].compare("demo2")){
        ActionSolver(support::splitLine("nuevo DemoModel2", " "));
        ActionSolver(support::splitLine("seleccionar DemoModel2", " "));
        ActionSolver(support::splitLine("vars corre alimentacion estudia feliz", " "));
        ActionSolver(support::splitLine("vals corre si no", " "));
        ActionSolver(support::splitLine("vals alimentacion buena mala", " "));
        ActionSolver(support::splitLine("vals estudia si no", " "));
        ActionSolver(support::splitLine("vals feliz si no", " "));
        ActionSolver(support::splitLine("data ./DemoData2.csv -h", " "));
        ActionSolver(support::splitLine("alpha 1", " "));
        ActionSolver(support::splitLine("fuerzabruta -k2", " "));
        ActionSolver(support::splitLine("k2 -k2 2", " "));
        ActionSolver(support::splitLine("k2 -aic 2", " "));
        ActionSolver(support::splitLine("k2 -mdl 2", " "));
        return;
    }
    if(!data[0].compare("demo3")){
        ActionSolver(support::splitLine("nuevo DemoModel3", " "));
        ActionSolver(support::splitLine("seleccionar DemoModel3", " "));
        ActionSolver(support::splitLine("vars nivel presicion entrena duerme", " "));
        ActionSolver(support::splitLine("vals nivel pesimo malo medio bueno excelente", " "));
        ActionSolver(support::splitLine("vals presicion baja media alta", " "));
        ActionSolver(support::splitLine("vals entrena nada poco medio bastante", " "));
        ActionSolver(support::splitLine("vals duerme si no", " "));
        ActionSolver(support::splitLine("data ./DemoData3.csv -h", " "));
        ActionSolver(support::splitLine("alpha 1", " "));
        ActionSolver(support::splitLine("fuerzabruta -k2", " "));
        ActionSolver(support::splitLine("k2 -k2 2", " "));
        ActionSolver(support::splitLine("k2 -aic 2", " "));
        ActionSolver(support::splitLine("k2 -mdl 2", " "));
        return;
    }
    if(!data[0].compare("demo4")){
        ActionSolver(support::splitLine("nuevo DemoModel1", " "));
         ActionSolver(support::splitLine("seleccionar DemoModel1", " "));
         ActionSolver(support::splitLine("vars A S T L B E X D", " "));
         ActionSolver(support::splitLine("vals A yes no", " "));
         ActionSolver(support::splitLine("vals S yes no", " "));
         ActionSolver(support::splitLine("vals T yes no", " "));
         ActionSolver(support::splitLine("vals L yes no", " "));
         ActionSolver(support::splitLine("vals B yes no", " "));
         ActionSolver(support::splitLine("vals E yes no", " "));
         ActionSolver(support::splitLine("vals X yes no", " "));
         ActionSolver(support::splitLine("vals D yes no", " "));
         ActionSolver(support::splitLine("data ./asia.csv -h", " "));
        ActionSolver(support::splitLine("alpha 1", " "));
        ActionSolver(support::splitLine("k2 -k2 1", " "));
        ActionSolver(support::splitLine("k2 -mdl 1", " "));
        ActionSolver(support::splitLine("chowliu", " "));
        ActionSolver(support::splitLine("dag", " "));
        ActionSolver(support::splitLine("entrenar", " "));
        ActionSolver(support::splitLine("conjunta S X", " "));
        ActionSolver(support::splitLine("condicional T -p B X", " "));
        ActionSolver(support::splitLine("inferencia T B A S X -v yes no no yes", " "));
        ActionSolver(support::splitLine("inferencia A B T -v yes no", " "));
        return;
    }
    if(!data[0].compare("entrenar")){
        engine::fit(current);
        std::cout << "Modelo Entrenado" << std::endl;
        return;
    }
    if(!data[0].compare("chowliu")){
        current->dag = engine::ChowLiu(current);
        return;
    }
    if(!data[0].compare("inferencia")){
        bool flag = true;
        std::vector<unsigned int> vars;
        std::vector<int> vals = {-1};
        unsigned int i = 1;
        for(; i < data.size(); i++){
            if(!data[i].compare("-v")){
                i += 1;
                break;
            }
            int subIndex = support::find(current->variables, data[i]);
            if(subIndex < 0){
                std::cout << "No existe la variable " << data[i] << std::endl;
                return;
            }
            vars.push_back(subIndex);
        }
        if(flag){
            int j = 1;
            for(; i < data.size(); i++){
                int subIndex = support::find(current->values[vars[j]], data[i]);
                j += 1;
                if(subIndex < 0){
                    std::cout << "No existe la valor " << data[i] << std::endl;
                    return;
                }
                vals.push_back(subIndex);
            }
        }
        engine::variableElimination(vars, vals, current);
        return;
    }
    std::cout << "Comando no existente" << std::endl;
    return;
}

int main(int argc, const char * argv[]) {
    std::cout << "Bienvenido: Escribe 'demo' para correr la demo o revisa la documentacion en el archivo 'manual.docx'" << std::endl;
    std::cout << ">> ";
    /*ActionSolver(support::splitLine("nuevo DemoModel1", " "));
    ActionSolver(support::splitLine("seleccionar DemoModel1", " "));
    ActionSolver(support::splitLine("vars A S T L B E X D", " "));
    ActionSolver(support::splitLine("vals A yes no", " "));
    ActionSolver(support::splitLine("vals S yes no", " "));
    ActionSolver(support::splitLine("vals T yes no", " "));
    ActionSolver(support::splitLine("vals L yes no", " "));
    ActionSolver(support::splitLine("vals B yes no", " "));
    ActionSolver(support::splitLine("vals E yes no", " "));
    ActionSolver(support::splitLine("vals X yes no", " "));
    ActionSolver(support::splitLine("vals D yes no", " "));
    ActionSolver(support::splitLine("data ./asia.csv -h", " "));
    ActionSolver(support::splitLine("k2 -k2", " "));
    //ActionSolver(support::splitLine("chowliu", " "));
    //ActionSolver(support::splitLine("dag", " "));
    //ActionSolver(support::splitLine("entrenar", " "));
    //ActionSolver(support::splitLine("inferencia T B A S X -v yes no no yes", " "));
    //ActionSolver(support::splitLine("inferencia S B -v no", " "));
    ActionSolver(support::splitLine("vars 0 1 2", " "));
    ActionSolver(support::splitLine("vals 0 0 1", " "));
    ActionSolver(support::splitLine("vals 1 0 1", " "));
    ActionSolver(support::splitLine("vals 2 0 1", " "));
    ActionSolver(support::splitLine("data ./data.csv", " "));*/
    //ActionSolver(support::splitLine("alpha 1", " "));
    //ActionSolver(support::splitLine("fuerzabruta -k2", " "));
    //engine::MaxLikelihood({0}, {-1}, current);
    //clock_t t = clock();
    //current->dag = engine::ChowLiu(current);
    //current->dag = engine::QM2::BruteForceNew(0, {3,2,1,0}, current);
    //current->dag = engine::QM2::K2N(0, {3,2,1,0}, current);
    //current->dag = engine::BruteForce(0, {0,1,2,3}, current);
    //current->dag = engine::QM2::K2N(0, {0,1,2,3}, current);
    //std::cout << (float)((clock() - t))/(CLOCKS_PER_SEC) << std::endl;
    //current->printDAG();
    //engine::fit(current);
    //ActionSolver(support::splitLine("inferencia class age marital_status -v 2 1", " "));
    //ActionSolver(support::splitLine("inferencia marital_status age educational_level -v 3 4", " "));
    
    
    
    //engine::QM2::K2N(1, {0, 1, 2}, current);
    //engine::QM2::K2N(2, {0, 1, 2}, current);
    //engine::QM2::BruteForceNew({0, 1, 2}, current);
    
    ResponseGroup<std::vector<std::string> > response = parser::ReadInputs();
    while (response.scores[0]) {
        ActionSolver(response.res[0]);
        std::cout << ">> ";
        response = parser::ReadInputs();
    }
    return 0;
}
