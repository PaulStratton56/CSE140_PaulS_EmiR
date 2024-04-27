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
    bool DEBUG = true;
    DataMemory* d_mem;
    RegisterFile* rf;
    ControlUnit* CU;
    std::string instructionFilePath;
    const int MEMORY_SIZE = 32; //Must be an even number.
    int pc;
    int ALUZero;
    int branchAddress;
    int total_clock_cycles = 1;
    std::vector<Line*> instructions;
    int ALUResult;  // Execute Result for ALU
    int ReadData;   // Memory Result for Read Data

public:
    Simulation(std::string instructionFilePath){
        d_mem = new DataMemory(MEMORY_SIZE);

        if(instructionFilePath == "sample_part1.txt"){
            d_mem->setData(hexToDec("0x70")/4,hexToDec("0x5"));
            d_mem->setData(hexToDec("0x74")/4,hexToDec("0x10"));
        }
        else if (instructionFilePath == "sample_part2.txt"){
            // Do nothing, all 0!
        }
        
        rf = new RegisterFile(MEMORY_SIZE);

        if(instructionFilePath == "sample_part1.txt"){
            rf->setData(1,hexToDec("0x20"));
            rf->setData(2,hexToDec("0x5"));
            rf->setData(10,hexToDec("0x70"));
            rf->setData(11,hexToDec("0x4"));
        }
        else if (instructionFilePath == "sample_part2.txt"){
            rf->setData(s0,hexToDec("0x20"));
            rf->setData(a0,hexToDec("0x05"));
            rf->setData(a1,hexToDec("0x2"));
            rf->setData(a2,hexToDec("0xa"));
            rf->setData(a3,hexToDec("0xf"));
        }
        

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
        pc is modified to 0x4   // 
        */

        while(pc < instructions.size() * 4){
            std::cout << "total_clock_cycles " << total_clock_cycles << " :" << std::endl;

            std::string instructionBinary = fetch(pc);
            
            Instruction decodedInstruction = decode(instructionBinary);

            ALUResult = execute(decodedInstruction);

            ReadData = memory(decodedInstruction);

            writeback(decodedInstruction);
            
            std::cout << "pc is modified to 0x" << std::hex << pc << std::endl << std::endl;
        }

        std::cout << "program terminated:" << std::endl << "total execution time is " << total_clock_cycles-1 << " cycles" << std::endl;
            
    }

    ~Simulation(){
        delete d_mem;
        delete rf;
        delete CU;
    }

private:

    std::string fetch(int instructionAddress){

        std::string result = instructions[(int)(instructionAddress/4)]->value;
        pc += 4; 

        return result;
    }

    Instruction decode(std::string instructionBinary){
        
        Instruction decodedInstruction(instructionBinary);
        CU->setSignals(decodedInstruction);

        if(DEBUG){
            decodedInstruction.printInfo();
            CU->printSignals();
        }

        return decodedInstruction;
    }

    int execute(Instruction instruction){
        int src1 = (instruction.getType() == UJ) ? pc : rf->getData(instruction.fieldData()["rs1"]);
        int src2 = (CU->getSignal(ALUSrc) == 1) ? instruction.fieldData()["imm"] : rf->getData(instruction.fieldData()["rs2"]);
        ALUCtrlValue ALUCtrl = getALUCtrl(instruction);
        int result = 0;
        switch(ALUCtrl){
            case(ADD):
                if(DEBUG){std::cout << "Performing ADD on " << ((instruction.getType() == UJ) ? "PC" : "rs1") << " and " << ((CU->getSignal(ALUSrc)) ? "imm" : "rs2") << std::endl;}
                result = src1 + src2;
                break;
            case(SUB):
                if(DEBUG){std::cout << "Performing SUB on " << ((instruction.getType() == UJ) ? "PC" : "rs1") << " and " << ((CU->getSignal(ALUSrc)) ? "imm" : "rs2") << std::endl;}
                result = src1 - src2;
                break;
            case(AND):
                if(DEBUG){std::cout << "Performing AND on " << ((instruction.getType() == UJ) ? "PC" : "rs1") << " and " << ((CU->getSignal(ALUSrc)) ? "imm" : "rs2") << std::endl;}
                result = src1 & src2;
                break;
            case(OR):
                if(DEBUG){std::cout << "Performing OR on " << ((instruction.getType() == UJ) ? "PC" : "rs1") << " and " << ((CU->getSignal(ALUSrc)) ? "imm" : "rs2") << std::endl;}
                result = src1 | src2;
                break;
            case(SLL):
                if(DEBUG){std::cout << "Performing SLL on " << ((instruction.getType() == UJ) ? "PC" : "rs1") << " and " << ((CU->getSignal(ALUSrc)) ? "imm" : "rs2") << std::endl;}
                result = src1 << src2;
                break;
            case(SRL):
                if(DEBUG){std::cout << "Performing SRL on " << ((instruction.getType() == UJ) ? "PC" : "rs1") << " and " << ((CU->getSignal(ALUSrc)) ? "imm" : "rs2") << std::endl;}
                result = (src1 >> src2) & ((int)pow(2, 32-src2) - 1); //Masks it with a bunch of zeroes at the beginning for logical shifting. Hooray!
                break;
            case(SRA):
                if(DEBUG){std::cout << "Performing SRA on " << ((instruction.getType() == UJ) ? "PC" : "rs1") << " and " << ((CU->getSignal(ALUSrc)) ? "imm" : "rs2") << std::endl;}
                result = src1 >> src2; //C++ does natural arithmetic shifting with >>.
                break;
            case(XOR):
                if(DEBUG){std::cout << "Performing XOR on " << ((instruction.getType() == UJ) ? "PC" : "rs1") << " and " << ((CU->getSignal(ALUSrc)) ? "imm" : "rs2") << std::endl;}
                result = src1 ^ src2;
                break;
            case(SLT):
                if(DEBUG){std::cout << "Performing SLT on " << ((instruction.getType() == UJ) ? "PC" : "rs1") << " and " << ((CU->getSignal(ALUSrc)) ? "imm" : "rs2") << std::endl;}
                result = (src1 < src2) ? 1 : 0;
                break;
        }

        ALUZero = (result == 0) ? 1 : 0;
        if(instruction.fieldData().count("imm") != 0){ //If the instruction has an immediate, use it as a branch address
            branchAddress = pc + (instruction.fieldData()["imm"] << 1);
            if(DEBUG){std::cout << "Updated branchTarget to " << branchAddress << std::endl;}
        }

        if(DEBUG){std::cout << "ALUResult: " << result << std::endl;}
        return result;
    }

    int memory(Instruction instruction){
        if(CU->getSignal(MemRead) == 1){
            // LW
            if(DEBUG){std::cout << "Retrieved 0x" << std::hex << d_mem->getData(ALUResult/4) << " from memory." << std::endl;}
            return d_mem->getData(ALUResult/4);
        }else if(CU->getSignal(MemWrite) == 1){
            // SW
            d_mem->setData(ALUResult/4, rf->getData(instruction.fieldData()["rs2"]));
            std::cout << "memory 0x" << std::hex << ALUResult << " is modified to 0x" << std::hex << rf->getData(instruction.fieldData()["rs2"]) << std::endl;
        }
        // Does nothing for R-Type and Branch
        return 0;
    }

    void writeback(Instruction instruction){
        /* == Write Back ==
        */
        if(CU->getSignal(RegWrite)){
            if(CU->getSignal(MemtoReg)){
                // Lw : Mem address
                rf->setData(instruction.fieldData()["rd"], ReadData);
                std::cout << "x" << std::hex << instruction.fieldData()["rd"] << " is modified to " << "0x" << std::hex << ReadData << std::endl;
            }else{
                //R-type : ALU address
                rf->setData(instruction.fieldData()["rd"], ALUResult);
                std::cout << "x" << std::hex << instruction.fieldData()["rd"] << " is modified to " << "0x" << std::hex << ALUResult << std::endl;
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
        MyFile.open(instructionFilePath);        
        std::string line;
        int pcNum = 0;

        if (!MyFile.is_open()) {
            std::cerr << "Failed to open the file." << std::endl;
        }

        while(getline(MyFile, line)){

            //Check if the string has whitespace..
            if(line.find_first_not_of("01") != std::string::npos){
                line = line.substr(line.find_first_of("01"),32);
            }

            instructions.push_back(new Line(line));
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

    int hexToDec(std::string hexValue){
        std::map<char, int> decValue;
        {decValue['0'] = 0;
        decValue['1'] = 1;
        decValue['2'] = 2;
        decValue['3'] = 3;
        decValue['4'] = 4;
        decValue['5'] = 5;
        decValue['6'] = 6;
        decValue['7'] = 7;
        decValue['8'] = 8;
        decValue['9'] = 9;
        decValue['a'] = 10;
        decValue['b'] = 11;
        decValue['c'] = 12;
        decValue['d'] = 13;
        decValue['e'] = 14;
        decValue['f'] = 15;}

        int result = 0;
        for(int i = 0; i < hexValue.length()-2; i++){

            result += decValue[(hexValue[hexValue.length()-1-i])] * pow(16,i);
        }
        return result;
    }

};




#endif