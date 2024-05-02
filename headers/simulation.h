#ifndef SIMULATION_H
#define SIMULATION_H

#include "memory.h"
#include "instruction.h"
#include "controlUnit.h"
#include "ALUCtrlValue.h"
#include "line.h"
#include <string>   
#include <iostream>


/* == Simulation ==
    Abstract class for a Single-cycle RISC-V CPU
    Used to call the Fetch, Decode, Execution, Memory, and WriteBack Components
*/
class Simulation{
    bool DEBUG = false;

    const int MEMORY_SIZE = 32; // Must be even.
    DataMemory* d_mem;
    RegisterFile* rf;
    ControlUnit* CU;

    std::string instructionFile;

    int pc;
    int ALUZero;
    int branchAddress;
    int total_clock_cycles;
    std::vector<Line*> instructions;
    int ALUResult;
    int ReadData;

public:
    /* == Constructs ==
        Sets up the Single-cycle RISC-V CPU
        Initializes Data memory, Regirster File, ControlUnit, PC, ALUZero, branchAddress, Total Clock Cycle, and Instruction File
        Fills in the Register File with predetermined Memory addresses
    */
    Simulation(std::string givenInstructionFile){
        d_mem = new DataMemory(MEMORY_SIZE);
        rf = new RegisterFile(MEMORY_SIZE);

        if(givenInstructionFile == "sample_part1.txt"){
            d_mem->setData(hexToDec("0x70")/4,hexToDec("0x5"));
            d_mem->setData(hexToDec("0x74")/4,hexToDec("0x10"));

            rf->setData(1,hexToDec("0x20"));
            rf->setData(2,hexToDec("0x5"));
            rf->setData(10,hexToDec("0x70"));
            rf->setData(11,hexToDec("0x4"));
        }
        else if (givenInstructionFile == "sample_part2.txt"){
            rf->setData(s0,hexToDec("0x20"));
            rf->setData(a0,hexToDec("0x05"));
            rf->setData(a1,hexToDec("0x2"));
            rf->setData(a2,hexToDec("0xa"));
            rf->setData(a3,hexToDec("0xf"));
        }
        
        CU = new ControlUnit();
        pc = 0;
        ALUZero = 0;
        branchAddress = 0;
        total_clock_cycles = 1;

        instructionFile = "instructionFiles/" + givenInstructionFile;
        readInstructionsFromFile();
    }

    /* == run() ==
        While the instruction size is less than the PC, we will Fetch, Decode, Execute, Memory, and WriteBack 
        (Depending on the Control Unit)
        Then decrease the total_clock_cycles by 1
    */
    void run(){

        while(pc < instructions.size() * 4){
            std::cout << "total_clock_cycles " << total_clock_cycles << " :" << std::endl;
            std::string instructionBinary = fetch(pc);
            
            Instruction decodedInstruction = decode(instructionBinary);

            ALUResult = execute(decodedInstruction);

            ReadData = memory(decodedInstruction);

            writeback(decodedInstruction);
            
            std::cout << "pc is modified to 0x" << std::hex << pc << std::dec << std::endl << std::endl;
        }

        std::cout << "program terminated:" << std::endl << "total execution time is " << total_clock_cycles-1 << " cycles" << std::endl;
            
    }

    /* == Deconstructor ==
        Deletes the Data memory, Register File, and Control Unit pointers
    */
    ~Simulation(){
        delete d_mem;
        delete rf;
        delete CU;
    }

private:

    /* == Fetch ==
        Gets Instruction from the Instruction Address
        Adds 4 to PC
    */
    std::string fetch(int instructionAddress){
        pc += 4;
        return getInstruction(instructionAddress);
    }

    /* == Decode ==
        Returns decoded Instruction 
        Sets signals on Control Unit
    */
    Instruction decode(std::string instructionBinary){
        
        Instruction decodedInstruction(instructionBinary);
        CU->setSignals(decodedInstruction);

        if(DEBUG){
            decodedInstruction.printInfo();
            CU->printSignals();
        }

        return decodedInstruction;
    }

    /* == Execute ==
        Returns retrieved/ computed register values and sign-extended offset values by using ALU 
    */
    //Note to self: JAL and JALR BOTH use the ALU to compute the target address.
    int execute(Instruction instruction){
        int src1;
        if(instruction.getType() == UJ){
            src1 = pc - 4;
        }
        else{
            src1 = getRegisterValue(instruction, "rs1");
        }

        int src2;
        if(CU->getSignal(ALUSrc) == 1){
            if(instruction.getType() == UJ){
                src2 = instruction.fieldData()["imm"] * 2;
            }
            else{
                src2 = instruction.fieldData()["imm"];
            }
        }
        else{
            src2 = getRegisterValue(instruction, "rs2");
        }

        ALUCtrlValue ALUCtrl = getALUCtrl(instruction);

        int result = compute(ALUCtrl, src1, src2, instruction);
        
        ALUZero = (result == 0) ? 1 : 0;

        if(instruction.getType() == SB){ 
            branchAddress = (pc-4) + (instruction.fieldData()["imm"] << 1);
            if(DEBUG){std::cout << "Updated branchTarget to " << branchAddress << std::endl;}
        }

        if(DEBUG){std::cout << "ALUResult: " << result << std::endl;}

        return result;
    }

    /* == Memory ==
        Based on the Control Unit, the memory will manipulate the Branch and Data Address
        If Branch and ALUZero, updates to branch Address
        If MemRead, returns Data memory
        If MemWrite, sets Data Memory to Data at Memory Address
    */
    int memory(Instruction instruction){
        //Get the next target for pc.
        if(ALUZero == 1 && CU->getSignal(Branch) == 1){
            pc = branchAddress;
            if(DEBUG){ std::cout << "Updated PC to " << branchAddress << " (branching)." << std::endl;}
        }

        if(CU->getSignal(MemRead) == 1){
            // LW
            if(DEBUG){std::cout << "Retrieved 0x" << std::hex << d_mem->getData(ALUResult/4) << std::dec << " from memory." << std::endl;}
            return d_mem->getData(ALUResult/4);
        }else if(CU->getSignal(MemWrite) == 1){
            // SW
            d_mem->setData(ALUResult/4, rf->getData(instruction.fieldData()["rs2"]));
            std::cout << "memory 0x" << std::hex << ALUResult << std::dec << " is modified to 0x" << std::hex << rf->getData(instruction.fieldData()["rs2"]) << std::dec << std::endl;
        }
        // Does nothing for R-Type and Branch
        return 0;
    }

    /* == Writeback ==
        Writes Data back into the Register if RegWrite is called in the Control Unit
        If isJump, it will set Rd as PC
        If MemtoReg, it will set Rd as Read Data
        Otherwise, it will set Rd as ALUResult

        Add one to the total_clock_cycles
    */
    void writeback(Instruction instruction){
        if(CU->getSignal(RegWrite)){
            if(isJump(instruction)){
                rf->setData(instruction.fieldData()["rd"], pc); //Set the return address.
                std::cout << "x" << instruction.fieldData()["rd"] << " is modified to " << "0x" << std::hex << pc << std::dec << std::endl;
                
                pc = ALUResult; //Jump to the address computed by the ALU.
            }else if(CU->getSignal(MemtoReg)){
                // Lw : Mem address
                rf->setData(instruction.fieldData()["rd"], ReadData);
                std::cout << "x" << instruction.fieldData()["rd"] << " is modified to " << "0x" << std::hex << ReadData << std::dec << std::endl;
            }else{
                //R-type : ALU address
                rf->setData(instruction.fieldData()["rd"], ALUResult);
                std::cout << "x" << instruction.fieldData()["rd"] << " is modified to " << "0x" << std::hex << ALUResult << std::dec << std::endl;
            }
        }
        // Does nothing for SW and Branch
        total_clock_cycles += 1;
    }

    /* == isJump ==
        Checks if the Instruction type Jumps
    */
    bool isJump(Instruction instruction){
        if(instruction.getType() == UJ || (instruction.getType() == I && instruction.fieldData()["opcode"] == 103)){
            return true;
        }
        return false;
    }

    /* == getRegisterValue ==
        Returns Data at given Register Name
    */
    int getRegisterValue(Instruction instruction, std::string registerName){
        return rf->getData(instruction.fieldData()[registerName]);
    }

    /* == compute ==
        Returns result of src1 and src2 based on ALUCtrl
    */
    int compute(ALUCtrlValue ALUCtrl, int src1, int src2, Instruction instruction){
        std::string debugOP = "";
        std::string debugSrc1 = "";
        std::string debugSrc2 = "";
        if(instruction.getType() == UJ){ debugSrc1 = "pc"; }
        else{ debugSrc1 = "rs1"; }
        if(CU->getSignal(ALUSrc) == 1){ debugSrc2 = "imm"; }
        else{ debugSrc2 = "rs2"; }
        
        int result = 0;
        switch(ALUCtrl){
            case(ADD):
                debugOP = "ADD";
                result = src1 + src2;
                break;
            case(SUB):
                debugOP = "SUB";
                result = src1 - src2;
                break;
            case(AND):
                debugOP = "AND";
                result = src1 & src2;
                break;
            case(OR):
                debugOP = "OR ";
                result = src1 | src2;
                break;
            case(SLL):
                debugOP = "SLL";
                result = src1 << src2;
                break;
            case(SRL):
                debugOP = "SRL";
                result = (src1 >> src2) & ((int)pow(2, 32-src2) - 1); //Masks it with a bunch of zeroes at the beginning for logical shifting. Hooray!
                break;
            case(SRA):
                debugOP = "SRA";
                result = src1 >> src2; //C++ does natural arithmetic shifting with >>.
                break;
            case(XOR):
                debugOP = "XOR";
                result = src1 ^ src2;
                break;
            case(SLT):
                debugOP = "SLT";
                result = (src1 < src2) ? 1 : 0;
                break;
        }

        if(DEBUG){ std::cout << "Performing " << debugOP << " on " << debugSrc1 << " and " << debugSrc2 << std::endl; }

        return result;
    }

    /* == ALUCtrl == 
        Returns ALUCtrl
    */
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

    /* == hexToDec ==
        Results decimal value of Hex number
    */
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

    /* == readInstructionsFromFile ==
        Stores each line of file to vector Array for easier access
    */
    void readInstructionsFromFile(){
        std::ifstream MyFile;
        MyFile.open(instructionFile);

        if (!MyFile.is_open()) {
            std::cerr << "Failed to open the file." << std::endl;
        }

        std::string line;
        while(getline(MyFile, line)){

            //Trim whitespace
            if(line.find_first_not_of("01") != std::string::npos){
                line = line.substr(line.find_first_of("01"),32);
            }

            instructions.push_back(new Line(line));
        }

        MyFile.close();
    }

    /* == getInstruction ==
        Returns Instruction
    */
    std::string getInstruction(int instructionAddress){
        return (instructions[(int)(instructionAddress/4)])->value;
    }

    /* == printData ==
        Prints Register File and Data Memory
    */
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

};




#endif