#ifndef SIMULATION_H
#define SIMULATION_H

#include "memory.h"
#include "instruction.h"
#include "controlUnit.h"
#include <string>   
#include <iostream>

class Simulation{
    DataMemory* d_mem;
    RegisterFile* rf;
    ControlUnit* CU;
    std::string instructionFilePath;
    const int MEMORY_SIZE = 32;
    int pc = 0;

public:
    Simulation(std::string instructionFilePath){
        d_mem = new DataMemory(MEMORY_SIZE);
        rf = new RegisterFile(MEMORY_SIZE);
        CU = new ControlUnit();
        this->instructionFilePath = instructionFilePath;
    }

    void run(){
        std::string instructionBinary = fetch(pc);

        Instruction decodedInstruction = decode(instructionBinary);

        execute(decodedInstruction);

        memory(decodedInstruction);

        writeback(decodedInstruction);
    }

    ~Simulation(){
        delete d_mem;
        delete rf;
        delete CU;
    }

private:

    std::string fetch(int pc){
        std::cout << "Fetching!" << std::endl;
        return "00000000010001010010000110000011";
    }

    Instruction decode(std::string instructionBinary){
        Instruction decodedInstruction(instructionBinary);
        CU->setSignals(decodedInstruction);
        return decodedInstruction;
    }

    void execute(Instruction instruction){
        std::cout << "Executing!" << std::endl;
    }

    void memory(Instruction instruction){
        std::cout << "Memory-ing!" << std::endl;
    }

    void writeback(Instruction instruction){
        std::cout << "Writing!" << std::endl;
    }

    /* ===================== */
    /* DEBUG FUNCTIONS BELOW */
    /* ===================== */

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