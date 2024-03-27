#ifndef SIMULATION_H
#define SIMULATION_H

#include "memory.h"
#include "instruction.h"
#include "controlUnit.h"
#include "ALUCtrlValue.h"
#include <string>   
#include <iostream>

class Simulation{
    DataMemory* d_mem;
    RegisterFile* rf;
    ControlUnit* CU;
    std::string instructionFilePath;
    const int MEMORY_SIZE = 32; //Must be an even number.
    int pc;
    int ALUZero;
    int branchAddress;

public:
    Simulation(std::string instructionFilePath){
        d_mem = new DataMemory(MEMORY_SIZE);
        rf = new RegisterFile(MEMORY_SIZE);
        CU = new ControlUnit();
        this->instructionFilePath = instructionFilePath;
        pc = 0;
        ALUZero = 0;
        branchAddress = 0;
    }

    void run(){
        std::string instructionBinary = fetch(pc);

        Instruction decodedInstruction = decode(instructionBinary);

        int ALUResult = execute(decodedInstruction);

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
        return "00000000001100101000001010110011";
    }

    Instruction decode(std::string instructionBinary){
        Instruction decodedInstruction(instructionBinary);
        CU->setSignals(decodedInstruction);
        return decodedInstruction;
    }

    int execute(Instruction instruction){
        int src1 = (instruction.getType() == UJ) ? pc : rf->getData(instruction.fieldData()["rs1"]);
        int src2 = (CU->getSignal(ALUSrc) == 1) ? rf->getData(instruction.fieldData()["imm"]) : rf->getData(instruction.fieldData()["rs2"]);
        ALUCtrlValue ALUCtrl = getALUCtrl(instruction);
        int result = 0;
        switch(ALUCtrl){
            case(ADD):
                std::cout << "Performing ADD on " << ((instruction.getType() == UJ) ? "PC" : "rs1") << " and " << ((CU->getSignal(ALUSrc)) ? "imm" : "rs2") << std::endl;
                result = src1 + src2;
                break;
            case(SUB):
                std::cout << "Performing SUB on " << ((instruction.getType() == UJ) ? "PC" : "rs1") << " and " << ((CU->getSignal(ALUSrc)) ? "imm" : "rs2") << std::endl;
                result = src1 - src2;
                break;
            case(AND):
                std::cout << "Performing AND on " << ((instruction.getType() == UJ) ? "PC" : "rs1") << " and " << ((CU->getSignal(ALUSrc)) ? "imm" : "rs2") << std::endl;
                result = src1 & src2;
                break;
            case(OR):
                std::cout << "Performing OR on " << ((instruction.getType() == UJ) ? "PC" : "rs1") << " and " << ((CU->getSignal(ALUSrc)) ? "imm" : "rs2") << std::endl;
                result = src1 | src2;
                break;
            case(SLL):
                std::cout << "Performing SLL on " << ((instruction.getType() == UJ) ? "PC" : "rs1") << " and " << ((CU->getSignal(ALUSrc)) ? "imm" : "rs2") << std::endl;
                result = src1 << src2;
                break;
            case(SRL):
                std::cout << "Performing SRL on " << ((instruction.getType() == UJ) ? "PC" : "rs1") << " and " << ((CU->getSignal(ALUSrc)) ? "imm" : "rs2") << std::endl;
                result = (src1 >> src2) & ((int)pow(2, 32-src2) - 1); //Masks it with a bunch of zeroes at the beginning for logical shifting. Hooray!
                break;
            case(SRA):
                std::cout << "Performing SRA on " << ((instruction.getType() == UJ) ? "PC" : "rs1") << " and " << ((CU->getSignal(ALUSrc)) ? "imm" : "rs2") << std::endl;
                result = src1 >> src2; //C++ does natural arithmetic shifting with >>.
                break;
            case(XOR):
                std::cout << "Performing XOR on " << ((instruction.getType() == UJ) ? "PC" : "rs1") << " and " << ((CU->getSignal(ALUSrc)) ? "imm" : "rs2") << std::endl;
                result = src1 ^ src2;
                break;
            case(SLT):
                std::cout << "Performing SLT on " << ((instruction.getType() == UJ) ? "PC" : "rs1") << " and " << ((CU->getSignal(ALUSrc)) ? "imm" : "rs2") << std::endl;
                result = (src1 < src2) ? 1 : 0;
                break;
        }

        ALUZero = (result == 0) ? 1 : 0;
        if(instruction.fieldData().count("imm") != 0){ //If the instruction has an immediate, use it as a branch address
            branchAddress = pc + (instruction.fieldData()["imm"] << 1);
            std::cout << "Updated branchTarget to " << branchAddress << std::endl;
        }

        return result;
    }

    void memory(Instruction instruction){
        std::cout << "Memory-ing!" << std::endl;
    }

    void writeback(Instruction instruction){
        std::cout << "Writing!" << std::endl;
    }

    ALUCtrlValue getALUCtrl(Instruction instruction){
        ALUCtrlValue ALUCtrl = NONE;
        switch(CU->getALUOP()){
            case(0): // S, UJ, or a "load" instruction.
                ALUCtrl = ADD;
                break;
            case(1): // SB
                ALUCtrl = SUB;
                break;
            case(2): // R or I
                if(instruction.getType() == R){ //R-Type depends on Func3/7 fields
                    switch(instruction.fieldData()["func3"]){
                        case(0): //Add or Sub, depending on func7
                            ALUCtrl = (instruction.fieldData()["func7"] == 0) ? ADD : SUB;
                            break;
                        case(7): //And
                            ALUCtrl = AND;
                            break;
                        case(6): //Or
                            ALUCtrl = OR;
                            break;
                    }
                }
                else if(instruction.getType() == I){ //I-type
                    ALUCtrl = (ALUCtrlValue)instruction.fieldData()["ALUCtrl"];
                    break;
                }
                else{ //Something's wrong..
                    std::cout << "Something went wrong with ALUCTRLVALUE. Oops!" << std::endl;
                }
                break;
        }
        return ALUCtrl;
    }

    /* ===================== */
    /* DEBUG FUNCTIONS BELOW */
    /* ===================== */

    void printData(){
        std::cout << "=== Register File ===" << std::endl;
        for(int i = 0; i < MEMORY_SIZE/2; i++){
            std::cout << "x" << i << ": " <<  rf->getData(i) <<  " | x" << i+(MEMORY_SIZE/2) << ": " <<  rf->getData(i+(MEMORY_SIZE/2)) << std::endl;
        }
        std::cout << "=== Data Memory ===" << std::endl;
        for(int i = 0; i < MEMORY_SIZE/2; i++){
            std::cout << "Mem[" << i << "]: " <<  d_mem->getData(i) << " | Mem[" << i+(MEMORY_SIZE/2) << "]: " <<  d_mem->getData(i+(MEMORY_SIZE/2)) << std::endl;
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