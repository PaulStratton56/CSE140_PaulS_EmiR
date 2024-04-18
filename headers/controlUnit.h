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

/* == ControlUnit ==
    Class to decipher all control signals of an instruction.
    Contains a list of all control signals, and ways to set and print them. */
class ControlUnit{
    bool* controlSignals;
    int ALUOP;
    const int SIGNALNUMBER = 6;

public:
    /* == ControlUnit() ==
        Default constructor. Initializes all control signals, setting all values to 0.
        This includes the ALUOP, which is not a boolean, but an integer. */
    ControlUnit(){
        controlSignals = new bool[SIGNALNUMBER];
        for(int signal = RegWrite; signal != End; signal++){
            controlSignals[signal] = 0;
        }

        ALUOP = 0;
    }

    /* == setSignals(vector<bool> newSignals, int newALUOP) ==
        Function Input:
            newSignals: vector of boolean signals to set the Control Unit's signal values to.
            newALUOP: integer to set the Control Unit's ALUOP to.

        Debug function. Takes in a manual list of control signals to set to, and force sets them. */
    void setSignals(std::vector<bool> newSignals, int newALUOP){
        if(newSignals.size() != SIGNALNUMBER){
            std::cout << "Cannot set new signals. newSignal length: " << newSignals.size() << "." << std::endl;
            return;
        }

        for(int signal = RegWrite; signal != End; signal++){
            controlSignals[signal] = newSignals[signal];
        }
        ALUOP = newALUOP;
    }

    /* == setSignals(Instruction instruction) ==
        Function Input:
            instruction: An Instruction type, used to decipher Control Signal values. 

        This function takes in a given instruction, and sets the values according to the type of instruction.
        Could be improved by changing the switch statement to a lookup function. 
        NOTE: The getALUOP function is unnecessary, but I kept it anyway. */
    void setSignals(Instruction& instruction){
        //Control Signals depend on the type of instruction.
        switch(instruction.getType()){ //Get instruction type
            case(R):
                controlSignals[RegWrite] = 1; //R-Types DO writeback.
                controlSignals[Branch]   = 0; //R-Types DO NOT branch.
                controlSignals[ALUSrc]   = 0; //R-Types DO NOT use immediates.
                controlSignals[MemWrite] = 0; //R-Types DO NOT write to memeory.
                controlSignals[MemtoReg] = 0; //R-Types DO NOT use values from memory.
                controlSignals[MemRead]  = 0; //R-Types DO NOT read from memory.
                ALUOP = instruction.getALUOP(); //R-Types use an ALUOP of 2.
                break;
            case(I):
                controlSignals[RegWrite] = 1; //I-Types DO writeback.
                controlSignals[Branch]   = 0; //I-Types DO NOT branch.
                controlSignals[ALUSrc]   = 1; //I-Types DO use immediates.
                controlSignals[MemWrite] = 0; //I-Types DO NOT write to memory.
                controlSignals[MemtoReg] = instruction.isLW() ? 1 : 0; //I-Types use memory values IF it is a "load" instruction.
                controlSignals[MemRead]  = instruction.isLW() ? 1 : 0; //I-Types read from memory IF it is a "load" instruction.
                ALUOP = instruction.isLW() ? 0 : instruction.getALUOP(); //I-Types are assumed to use an ALUOP of 2, unless it's a "load" instruction.
                break;
            case(S):
                controlSignals[RegWrite] = 0; //S-Types DO NOT writeback.
                controlSignals[Branch]   = 0; //S-Types DO NOT branch.
                controlSignals[ALUSrc]   = 1; //S-Types DO use immediates.
                controlSignals[MemWrite] = 1; //S-Types DO write to memory.
                // controlSignals[MemtoReg];  //S-Types DO NOT USE either writeback value. Irrelevant.
                controlSignals[MemRead]  = 0; //S-Types DO NOT read from memory.
                ALUOP = instruction.getALUOP(); //S-Types use an ALUOP of 0.
                break;
            case(SB):
                controlSignals[RegWrite] = 0; //SB-Types DO NOT writeback.
                controlSignals[Branch]   = 1; //SB-Types DO branch.
                controlSignals[ALUSrc]   = 0; //SB-Types DO NOT use immediates. (ALU compares two registers)
                controlSignals[MemWrite] = 0; //SB-Types DO NOT write to memory.
                // controlSignals[MemtoReg];  //SB-Types DO NOT USE either writeback value. Irrelevant.
                controlSignals[MemRead]  = 0; //SB-Types DO NOT read from memory.
                ALUOP = instruction.getALUOP(); //SB-Types use an ALUOP of 1.
                break;
            case(U):
                std::cout << "U Type?? AAAAH" << std::endl;
                break;
            case(UJ):
                controlSignals[RegWrite] = 1; //UJ-Types DO write return address to a register.
                controlSignals[Branch]   = 0; //UJ-Types DO NOT branch.
                controlSignals[ALUSrc]   = 1; //UJ-Types DO use immediates to calculate the new address.
                controlSignals[MemWrite] = 0; //UJ-Types DO NOT write to memory.
                controlSignals[MemtoReg] = 0; //UJ-Types DO NOT use values from memory.
                controlSignals[MemRead]  = 0; //UJ-Types DO NOT read from memory.
                ALUOP = instruction.getALUOP(); //UJ-Types are assumed to use an ALUOP of 0, as it always performs addition.
                break;
        };
    }

    bool getSignal(Signal signal){
        return controlSignals[signal];
    }

    int getALUOP(){
        return ALUOP;
    }
    
    /* == printSignals() ==
        Prints all relevant signals from the Control Unit. */
    void printSignals(){
        std::cout << "===== SIGNALS =====" << std::endl;
        for(int signal = RegWrite; signal != End; signal++){
            std::cout << signalNames[signal] << " : " << controlSignals[signal] << std::endl;
        }
        std::cout << "ALUOP : " << ALUOP << std::endl; 
        std::cout << "===================" << std::endl;
    }

    ~ControlUnit(){
        delete[] controlSignals;
    }

};

#endif