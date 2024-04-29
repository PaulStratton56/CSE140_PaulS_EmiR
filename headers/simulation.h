#ifndef SIMULATION_H
#define SIMULATION_H

#include "memory.h"
#include "instruction.h"
#include "controlUnit.h"
#include "ALUCtrlValue.h"
#include "line.h"
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
    int total_clock_cycles = 0;
    std::vector<Line*> texts;
    int ALUResult;  // Execute Result for ALU
    int ReadData;   // Memory Result for Read Data

public:
    Simulation(std::string instructionFilePath){
        d_mem = new DataMemory(MEMORY_SIZE);
        // Initialize d_mem
        /*
        0x70 = 0x5
        0x74 = 0x10
        */
        rf = new RegisterFile(MEMORY_SIZE);
        // Initialized Values
        /*
        x1 = 0x20
        x2 = 0x5
        x10 = 0x70
        x11 = 0x4
        */
        CU = new ControlUnit();
        this->instructionFilePath = "instructionFiles/" + instructionFilePath;
        pc = 0;
        ALUZero = 0;
        branchAddress = 0;

        fillFile();
    }

    void run(){
        /*
        Change to make it look like the proper outcome
        ex:
        total_clock_cycle = # :
        x3 is modified to 0x10  // Execute
        pc is modified to 0x4   // WriteBack
        */

        for(int i = 0; i < texts.size(); i++){
            std::cout << "total_clock_cycles " << total_clock_cycles << " :" << std::endl;
            
            std::string instructionBinary = fetch(total_clock_cycles);
            std::cout << instructionBinary << std::endl;
            
            Instruction decodedInstruction = decode(instructionBinary);

            ALUResult = execute(decodedInstruction);

            ReadData = memory(decodedInstruction);

            writeback(decodedInstruction);
        }

        std::cout << "total_clock_cycles " << total_clock_cycles << " :" << std::endl;
        std::cout << "END" << std::endl;
            
    }

    ~Simulation(){
        delete d_mem;
        delete rf;
        delete CU;
    }

private:

    std::string fetch(int pc){
        std::cout << "Fetching!" << std::endl;

        std::string result = texts[pc]->value;
        pc = pc +1; // Change this to pc + 4 and make it so that everything works with it

        return result;
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

    int memory(Instruction instruction){
        std::cout << "Memory-ing!" << std::endl;
        /* == Memory Access ==
        */
        if(CU->getSignal(MemRead)){
            // LW
            return d_mem->getData(ALUResult);
        }else if(CU->getSignal(MemWrite)){
            // SW
            d_mem->setData(ALUResult, rf->getData(instruction.fieldData()["rs2"]));
        }
        // Does nothing for R-Type and Branch
        return 0;
    }

    void writeback(Instruction instruction){
        std::cout << "Writing!" << std::endl;
        /* == Write Back ==
        */
        if(CU->getSignal(RegWrite)){
            if(CU->getSignal(MemtoReg)){
                // Lw : Mem address
                rf->setData(instruction.fieldData()["rd"], ReadData);
            }else{
                //R-type : ALU address
                rf->setData(instruction.fieldData()["rd"], ALUResult);
            }
        }
        // Does nothing for SW and Branch
        total_clock_cycles += 1;
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

    void fillFile(){
        std::ifstream MyFile;
        std::cout << "Filepath: " + instructionFilePath << std::endl;
        MyFile.open(instructionFilePath);        
        std::string line;
        int pcNum = 0;

        if (!MyFile.is_open()) {
            std::cerr << "Failed to open the file." << std::endl;
        }

        while(getline(MyFile, line)){
            texts.push_back(new Line(line));
            std::cout << texts[pcNum/4]->getValue() << std::endl;
            pcNum = pcNum +4;
        }

        MyFile.close();
    }

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