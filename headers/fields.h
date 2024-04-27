#ifndef FIELDS_H
#define FIELDS_H

#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <map>
#include "ALUCtrlValue.h"

/* == Field ==
    Abstract class all field classes derive from.
    Used to upcast any kind of field to be stored in a pointer inside an Instruction object. */
class Field{
public:
    /* == printInfo ==
        Prints the values of each child class's field, plus operation and instruction type.
        Abstract method in Field class. */
    virtual void printInfo() = 0;
    /* == info ==
        Returns a "dict" of all relevant information (registers, etc).
        Note the keys will change depending on the instruction type. */
    virtual std::map<std::string, int> info() = 0;
    /* == isLW ==
        Returns whether the given instruction is a LW instruction.
        Overriden ONLY by the I-Type field. */
    virtual bool isLW(){ return false; }
    /* == getALUOP ==
        Returns a 2b integer specifying the ALUOP for the control signal.
        Note these are inferred for some instructions.
        Abstract method in Field class. */
    virtual int getALUOP() = 0;


protected:

    /* == binToInt(std::vector<int> binary, std::vector<int> indices = {}, bool checkSign = false) ==
        Input:
            binary: vector of integers representing binary digits, with lsb at index 0.
            indices: optional list of indices to pull digits from in the binary vector, starting at indices[0].
                Useful if the resulting decimal value does not directly correspond to the binary vector.
                Defaults to using the entire vector, starting from lsb.
            checkSign: optional boolean. True if using two's complement, defaults to false if unsigned.
        
        Output:
            Integer that the binary describes.
            
        Takes a vector of binary digits and optional parameters to convert to an integer.*/
    int binToInt(std::vector<int> binary, std::vector<int> indices = {}, bool checkSign = false){
        int result = 0;
        if(!checkSign){ //Unsigned conversion
            if(indices.size() == 0){ //No index provided, use whole vector
                for(int i = 0; i < binary.size(); i++){
                    result += binary[i] * pow(2,i);
                }
            }
            else{ //Index provided, pull from indices
                for(int i = 0; i < indices.size(); i++){
                    result += binary[indices[i]] * pow(2,i);
                }
            }
        }
        else{ //Signed conversion
            if(indices.size() == 0){ //No index provided, use whole vector
                if(binary[binary.size()-1] == 0){ //Non-negative, convert as normal.
                    for(int i = 0; i < binary.size(); i++){
                        result += binary[i] * pow(2,i);
                    }
                }
                else{ //Negative, convert inverse and add 1, then make negative
                    for(int i = 0; i < (binary.size()-1); i++){ //Size-1 to ignore sign bit
                        result += ((binary[i] + 1) % 2) * pow(2,i); //Flip bits using %2
                    }
                    result += 1; //Add 1 (otherwise 1's complement)
                    result *= -1; //Flip because negative!
                }
            }
            else{ //Index provided, pull from indices
                if(binary[indices[indices.size()-1]] == 0){ //No sign necessary, convert as normal
                    for(int i = 0; i < indices.size(); i++){
                        result += binary[indices[i]] * pow(2,i);
                    }
                }
                else{ //Negative, convert inverse and add 1, then make negative
                    for(int i = 0; i < (indices.size()-1); i++){ //Size-1 to ignore sign bit
                        result += ((binary[indices[i]] + 1) % 2) * pow(2,i); //Flip bits using %2
                    }
                    result += 1; //Add 1 (otherwise 1's complement)
                    result *= -1; //Flip because negative!
                }
            }
        }
        return result;
    }
};

/* == RField ==
    Class for R-Type instructions.
    Contains information about fields, and instruction operation. */
class RField: public Field{
public:
    /* == RField(vector<int> binaryCode) ==
        Constructor Parameters:
            binaryCode: Vector of integers representing binary code with lsb at index 0. 

        Default constructor. Takes binary code, and gathers R-Type instruction information.
        This information can be printed using the printInfo member function. */
    RField(std::vector<int> binaryCode){
        //Get field information
        rs2   = binToInt(binaryCode, {20, 21, 22, 23, 24});
        rs1   = binToInt(binaryCode, {15, 16, 17, 18, 19});
        rd    = binToInt(binaryCode, { 7,  8,  9, 10, 11});
        func3 = binToInt(binaryCode, {12, 13, 14});
        func7 = binToInt(binaryCode, {25, 26, 27, 28, 29, 30, 31});
        
        //Get instruction operation
        if(func7 == 32){ // 0100000
            switch(func3){
                case(0): //sub (3: 000 7: 0100000)
                    operation = "sub";
                    break;
                case(5): //sra (3: 101 7: 0100000)
                    operation = "sra";
                    break;
            }
        }
        else if(func7 == 0){ // 0000000
            switch(func3){
                case(0): //add (3: 000 7: 0000000)
                    operation = "add";
                    break;
                case(1): //sll (3: 001 7: 0000000)
                    operation = "sll";
                    break;
                case(2): //slt (3: 010 7: 0000000)
                    operation = "slt";
                    break;
                case(3): //sltu (3: 011 7: 0000000)
                    operation = "sltu";
                    break;
                case(4): //xor (3: 100 7: 0000000)
                    operation = "xor";
                    break;
                case(5): //srl (3: 101 7: 0000000)
                    operation = "srl";
                    break;
                case(6): //or (3: 110 7: 0000000)
                    operation = "or";
                    break;
                case(7): //and (3: 111 7: 0000000)
                    operation = "and";
                    break;
            }
        }
        else{
            std::cout << "WARNING: Invalid func7 field: " << func7 << ". Something went wrong" << std::endl;
        }

    }

    /* == getALUOP ==
        For R-Type instructions, the ALUOP is always 10 in binary.
        The ALU operation is determined by the Funct3/Funct7 fields. */
    int getALUOP(){
        return 2;
    }

    std::map<std::string, int> info(){
        std::map<std::string, int> info;
        info["rs1"] = rs1;
        info["rs2"] = rs2;
        info["rd"] = rd;
        info["func3"] = func3;
        info["func7"] = func7;
        return info;
    }

    /* == printInfo ==
        Prints the values of each R-Type field, plus operation and instruction type.*/
    void printInfo(){
        std::cout << "Instruction Type: R" << std::endl;
        std::cout << "Operation: " << operation << std::endl;
        std::cout << "Rs1: x" << rs1 << std::endl;
        std::cout << "Rs2: x" << rs2 << std::endl;
        std::cout << "Rd: x" << rd << std::endl;
        std::cout << "Func3: " << func3 << std::endl;
        std::cout << "Func7: " << func7 << std::endl;
    }

    ~RField(){}

private:
    std::string operation;
    int rs1, rs2, rd, func3, func7;
};

//NOTE: This uses 2's complement for the immediate in all cases BUT sltiu. Consider checking which functions use 2's complement vs unsigned.
/* == IField ==
    Class for I-Type instructions.
    Contains information about fields, and instruction operation. */
class IField: public Field{
public:
    /* == IField(vector<int> binaryCode) ==
        Constructor Parameters:
            binaryCode: Vector of integers representing binary code with lsb at index 0. 

        Default constructor. Takes binary code, and gathers I-Type instruction information.
        This information can be printed using the printInfo member function. */
    IField(std::vector<int> binaryCode){
        //Get field information
        imm   = binToInt(binaryCode, {20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31}, true);
        opcode= binToInt(binaryCode, { 0,  1,  2,  3,  4,  5,  6});
        rs1   = binToInt(binaryCode, {15, 16, 17, 18, 19});
        rd    = binToInt(binaryCode, { 7,  8,  9, 10, 11});
        func3 = binToInt(binaryCode, {12, 13, 14});

        //Get instruction operation
        if(opcode == 19){ //0010011
            switch(func3){
                case(0): //addi (op: 0010011 func3: 000)
                    operation = "addi";
                    ALUCtrl = ADD;
                    break;
                case(1): //slli (op: 0010011 func3: 001)
                    operation = "slli";
                    ALUCtrl = SLL;
                    break;
                case(2): //slti (op: 0010011 func3: 010)
                    operation = "slti";
                    ALUCtrl = SLT;
                    break;
                case(3): //sltiu(op: 0010011 func3: 011)
                    operation = "sltiu";
                    ALUCtrl = SLT;
                    imm = binToInt(binaryCode, {20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30}, false); //reassign because not signed
                    break;
                case(4): //xori (op: 0010011 func3: 100)
                    operation = "xori";
                    ALUCtrl = XOR;
                    break;
                case(5): //Either srai or srli
                    if(binToInt(binaryCode, {30}) == 1){ //srai (op: 0010011 func3: 101 (func7: 0100000))
                        operation = "srai";
                        ALUCtrl = SRA;
                    }
                    else if(binToInt(binaryCode, {30}) == 0){ //srli (op: 0010011 func3: 101 (func7: 0000000))
                        operation = "srli";
                        ALUCtrl = SRL;
                    }
                    else{
                        std::cout << "WARNING: non-binary func7 field. Something went HORRIBLY wrong." << std::endl;
                    }
                    break;
                case(6): //ori  (op: 0010011 func3: 110)
                    operation = "ori";
                    ALUCtrl = OR;
                    break;
                case(7): //andi (op: 0010011 func3: 111)
                    operation = "andi";
                    ALUCtrl = AND;
                    break;
            }
        }
        else if(opcode == 3){ //0000011
            switch(func3){
                case(0): //lb   (op: 0000011 func3: 000)
                    operation = "lb";
                    ALUCtrl = ADD;
                    break;
                case(1): //lh   (op: 0000011 func3: 001)
                    operation = "lh";
                    ALUCtrl = ADD;
                    break;
                case(2): //lw   (op: 0000011 func3: 010)
                    operation = "lw";
                    ALUCtrl = ADD;
                    break;
            }
        }
        else if(opcode == 103){
            switch(func3){
                case(0): //jalr (op: 1100111 func3: 000)
                    operation = "jalr";
                    ALUCtrl = ADD;
                    break;
            }
        }
        else{ 
            std::cout << "WARNING: Invalid opcode field: " << opcode << ". Something went wrong" << std::endl;
        }
    }
    
    /* == getALUOP ==
        For I-Type instructions, we assume the value to be 2, as I-Type instructions must cover a range of ALU operations.
        This could be incorrect, but is a "best guess" for now. */
    int getALUOP(){
        return 2;
    }

    /* == isLW ==
        Returns true if this I-Type instruction is a "load" instruction, and false otherwise. */
    bool isLW(){
        return (operation == "lw" || operation == "lh" || operation == "lb") ? true : false;
    }

    std::map<std::string, int> info(){
        std::map<std::string, int> info;
        info["rs1"] = rs1;
        info["rd"] = rd;
        info["imm"] = imm;
        info["func3"] = func3;
        info["opcode"] = opcode;
        info["ALUCtrl"] = ALUCtrl;
        return info;
    }

    /* == printInfo ==
        Prints the values of each R-Type field, plus operation and instruction type.*/
    void printInfo(){
        std::cout << "Instruction Type: I" << std::endl;
        std::cout << "Operation: " << operation << std::endl;
        std::cout << "Rs1: x" << rs1 << std::endl;
        std::cout << "Rd: x" << rd << std::endl;
        if(imm >= 10 || imm <= -10){ //Hex needed
            std::cout << "imm: " << imm << " (or 0x" << std::hex << imm << std::dec << ")" << std::endl;
        }
        else{
            std::cout << "imm: " << imm << std::endl;
        }
    }

    ~IField(){}

private:
    std::string operation;
    int rs1, rd, imm, func3, opcode;
    ALUCtrlValue ALUCtrl;

};

//NOTE: This ALWAYS uses 2's complement for the immediate. Consider checking which functions use 2's complement vs unsigned.
/* == SField ==
    Class for S-Type instructions.
    Contains information about fields, and instruction operation. */
class SField: public Field{
public:
    /* == SField(vector<int> binaryCode) ==
        Constructor Parameters:
            binaryCode: Vector of integers representing binary code with lsb at index 0. 

        Default constructor. Takes binary code, and gathers S-Type instruction information.
        This information can be printed using the printInfo member function. */
    SField(std::vector<int> binaryCode){
        //Get field information
        rs1  = binToInt(binaryCode, {15, 16, 17, 18, 19});
        rs2  = binToInt(binaryCode, {20, 21, 22, 23, 24});
        func3= binToInt(binaryCode, {12, 13, 14});
        imm  = binToInt(binaryCode, {7, 8, 9, 10, 11, 25, 26, 27, 28, 29, 30, 31}, true); //If considering signed is conditional, put this into the switch statment.

        //Get instruction operation
        switch(func3){
            case(0): //sb (func3: 0)
                operation = "sb";
                break;
            case(1): //sh (func3: 1)
                operation = "sh";
                break;
            case(2): //sw (func3: 2)
                operation = "sw";
                break;
            default:
                std::cout << "WARNING: Invalid func3 field: " << func3 << ". Something went wrong" << std::endl;
                break;
        }

    }

    /* == getALUOP ==
        For S-Type instructions, the ALUOP is always 0 in binary. */
    int getALUOP(){
        return 0;
    }

    std::map<std::string, int> info(){
        std::map<std::string, int> info;
        info["rs1"] = rs1;
        info["rs2"] = rs2;
        info["imm"] = imm;
        info["func3"] = func3;
        return info;
    }

    /* == printInfo ==
        Prints the values of each S-Type field, plus operation and instruction type.*/
    void printInfo(){
        std::cout << "Instruction Type: S" << std::endl;
        std::cout << "Operation: " << operation << std::endl;
        std::cout << "Rs1: x" << rs1 << std::endl;
        std::cout << "Rs2: x" << rs2 << std::endl;
        if(imm >= 10 || imm <= -10){ //Hex needed
            std::cout << "imm: " << imm << " (or 0x" << std::hex << imm << std::dec << ")" << std::endl;
        }
        else{
            std::cout << "imm: " << imm << std::endl;
        }
    }
    
    ~SField(){}

private:
    std::string operation;
    int rs1, rs2, func3, imm;

};

//NOTE: This ALWAYS uses 2's complement for the immediate. Consider checking which functions use 2's complement vs unsigned.
/* == SBField ==
    Class for SB-Type instructions.
    Contains information about fields, and instruction operation. */
class SBField: public Field{
public:
    /* == SBField(vector<int> binaryCode) ==
        Constructor Parameters:
            binaryCode: Vector of integers representing binary code with lsb at index 0. 

        Default constructor. Takes binary code, and gathers SB-Type instruction information.
        This information can be printed using the printInfo member function. */
    SBField(std::vector<int> binaryCode){
        //Get field information
        rs1  = binToInt(binaryCode, {15, 16, 17, 18, 19});
        rs2  = binToInt(binaryCode, {20, 21, 22, 23, 24});
        func3= binToInt(binaryCode, {12, 13, 14});
        imm  = binToInt(binaryCode, { 8,  9, 10, 11, 25, 26, 27, 28, 29, 30, 7, 31}, true); //This is why we can't have nice things.
        //Oh no! However will I add in the 0 at the end of the immediate?
        imm *= 2; //Yeah. Take that.

        //Get instruction operation
        switch(func3){
            case(0): //beq (func3: 000 (0))
                operation = "beq";
                break;
            case(1): //bne (func3: 001 (1)) 
                operation = "bne";
                break;
            case(4): //blt (func3: 100 (4))
                operation = "blt";
                break;
            case(5): //bge (func3: 101 (5))
                operation = "bge";
                break;
        }

    }

    /* == getALUOP ==
        For SB-Type instructions, the ALUOP is always 1. */
    int getALUOP(){
        return 1;
    }

    std::map<std::string, int> info(){
        std::map<std::string, int> info;
        info["rs1"] = rs1;
        info["rs2"] = rs2;
        info["imm"] = imm;
        info["func3"] = func3;
        return info;
    }

    /* == printInfo ==
        Prints the values of each SB-Type field, plus operation and instruction type.*/
    void printInfo(){
        std::cout << "Instruction Type: SB" << std::endl;
        std::cout << "Operation: " << operation << std::endl;
        std::cout << "Rs1: x" << rs1 << std::endl;
        std::cout << "Rs2: x" << rs2 << std::endl;
        if(imm >= 10 || imm <= -10){ //Hex needed
            std::cout << "imm: " << imm << " (or 0x" << std::hex << imm << std::dec << ")" << std::endl;
        }
        else{
            std::cout << "imm: " << imm << std::endl;
        }
    }

    ~SBField(){}

private:
    std::string operation;
    int rs1, rs2, func3, imm;

};

//NOTE: This ALWAYS uses 2's complement for the immediate. Consider checking 2's complement vs unsigned.
/* == UJField ==
    Class for UJ-Type instructions.
    Contains information about fields, and instruction operation. */
class UJField: public Field{
public:
    /* == UJField(vector<int> binaryCode) ==
        Constructor Parameters:
            binaryCode: Vector of integers representing binary code with lsb at index 0. 

        Default constructor. Takes binary code, and gathers UJ-Type instruction information.
        This information can be printed using the printInfo member function. */
    UJField(std::vector<int> binaryCode){
        rd = binToInt(binaryCode, {7, 8, 9, 10, 11});
        imm= binToInt(binaryCode, {21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 20, 12, 13, 14, 15, 16, 17, 18, 19, 31}); //Ew!

        operation = "jal"; //There's only one!
    }

    /* == getALUOP ==
        For UJ-Type instructions, we assume the ALU serves the purpose of adding the offset to its address.
        Because of this, it makes sense to have the ALUOP be 0, as it always results in the ALU performing addition. */
    int getALUOP(){
        return 0; //JAL adds the offset to the address of the instruction, so it should always add.
    }

    /* == printInfo ==
        Prints the values of each UJ-Type field, plus operation and instruction type.*/
    void printInfo(){
        std::cout << "Instruction Type: UJ" << std::endl;
        std::cout << "Operation: " << operation << std::endl;
        std::cout << "Rd: x" << rd << std::endl;
        if(imm >= 10 || imm <= -10){ //Hex needed
            std::cout << "imm: " << imm << " (or 0x" << std::hex << imm << std::dec << ")" << std::endl;
        }
        else{
            std::cout << "imm: " << imm << std::endl;
        }
    }

    std::map<std::string, int> info(){
        std::map<std::string, int> info;
        info["rd"] = rd;
        info["imm"] = imm;
        return info;
    }

    ~UJField(){}

private:
    std::string operation;
    int rd, imm;

};

#endif
