#ifndef SIMULATION_H
#define SIMULATION_H

#include "memory.h"
#include <string>
#include <iostream>

class Simulation{
    DataMemory* d_mem;
    RegisterFile* rf;
    std::string instructionFilePath;
    const int MEMORY_SIZE = 32;

public:
    Simulation(std::string instructionFilePath){
        d_mem = new DataMemory(MEMORY_SIZE);
        rf = new RegisterFile(MEMORY_SIZE);
        this->instructionFilePath = instructionFilePath;
    }

    void run(){
        std::cout << "Running!" << std::endl;
    }

    ~Simulation(){
        delete d_mem;
        delete rf;
    }

private:

    

    /*
    =====================
    DEBUG FUNCTIONS BELOW
    =====================
    */
    void printData(){
        std::cout << "=== Register File ===" << std::endl;
        for(int i = 0; i < MEMORY_SIZE/2; i++){
            std::cout << "x" << i << ": " <<  rf->getData(i) <<  " | x" << i+16 << ": " <<  rf->getData(i+16) << std::endl;
        }
        std::cout << "=== Data Memory ===" << std::endl;
        for(int i = 0; i < MEMORY_SIZE/2; i++){
            std::cout << "Mem[" << i << "]: " <<  d_mem->getData(i) << " | Mem[" << i+16 << "]: " <<  d_mem->getData(i+16) << std::endl;
        }
    }

    void setReg(Register reg, int val){
        rf->setData(reg, val);
    }

    void setMem(int index, int val){
        d_mem->setData(index, val);
    }


};




#endif