#ifndef CONTROLUNIT_H
#define CONTROLUNIT_H

#include <vector>
#include <string>
#include <iostream>

enum Signal{
    RegWrite,
    Branch,
    ALUSrc,
    MemWrite,
    MemtoReg,
    MemRead,
    End
};
std::vector<std::string> signalNames = {"RegWrite", "Branch", "ALUSrc", "MemWrite", "MemToReg", "MemRead"};

class ControlUnit{
    bool* controlSignals;
    int ALUOP;
    const int SIGNALNUMBER = 6;

public:
    ControlUnit(){
        controlSignals = new bool[SIGNALNUMBER];
        for(int signal = RegWrite; signal != End; signal++){
            controlSignals[signal] = 0;
        }

        ALUOP = 0;
    }

    void setSignals(std::vector<bool> newSignals){
        if(newSignals.size() != SIGNALNUMBER){
            std::cout << "Cannot set new signals. newSignal length: " << newSignals.size() << "." << std::endl;
            return;
        }

        for(int signal = RegWrite; signal != End; signal++){
            controlSignals[signal] = newSignals[signal];
        }
    }

    void setSignals(Instruction& instruction){
        switch(instruction.getType()){
            case(R):
                controlSignals[RegWrite] = 1;
                controlSignals[Branch] = 0;
                controlSignals[ALUSrc] = 0;
                controlSignals[MemWrite] = 0;
                controlSignals[MemtoReg] = 0;
                controlSignals[MemRead] = 0;
                ALUOP = instruction.getALUOP();
                break;
            case(I):
                controlSignals[RegWrite] = 1;
                controlSignals[Branch] = 0;
                controlSignals[ALUSrc] = 1;
                controlSignals[MemWrite] = 0;
                controlSignals[MemtoReg] = instruction.isLW() ? 1 : 0;
                controlSignals[MemRead] = instruction.isLW() ? 1: 0;
                ALUOP = instruction.getALUOP();
                break;
            case(S):
                controlSignals[RegWrite] = 0;
                controlSignals[Branch] = 0;
                controlSignals[ALUSrc] = 1;
                controlSignals[MemWrite] = 1;
                // controlSignals[MemtoReg]; Doesn't matter
                controlSignals[MemRead] = 0;
                ALUOP = instruction.getALUOP();
                break;
            case(SB):
                controlSignals[RegWrite] = 0;
                controlSignals[Branch] = 1; //duh
                controlSignals[ALUSrc] = 0;
                controlSignals[MemWrite] = 0;
                // controlSignals[MemtoReg]; Doesn't matter
                controlSignals[MemRead] = 0;
                ALUOP = instruction.getALUOP();
                break;
            case(U):
                std::cout << "U Type?? AAAAH" << std::endl;
                break;
            case(UJ):
                controlSignals[RegWrite] = 1; //Need to write the return address
                controlSignals[Branch] = 0; //Not a branch instruction.
                controlSignals[ALUSrc] = 1; //Need to add the immediate to the program counter, I assume
                controlSignals[MemWrite] = 0; //Not writing to memory
                controlSignals[MemtoReg] = 0; //Writing the result of the ALU (target address) to rd
                controlSignals[MemRead] = 0; //Not reading from data memory
                ALUOP = instruction.getALUOP();
                break;
        };
    }

    void printSignals(){
        std::cout << "===== SIGNALS =====" << std::endl;
        for(int signal = RegWrite; signal != End; signal++){
            std::cout << signalNames[signal] << " : " << controlSignals[signal] << std::endl;
        }
        std::cout << "===================" << std::endl;
    }

    ~ControlUnit(){
        delete[] controlSignals;
    }

};

#endif