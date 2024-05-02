#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <cmath>
#include "fields.h"

/* == InstructionType ==
    List of Instruction Types
*/
enum InstructionType{
    R,
    I,
    S,
    SB,
    U,
    UJ,
    END
};

/* == Instruction ==
    Class for Instruction
    Sets up binary list as string and integers, info about Field, and Instruction Type
*/
class Instruction{
    std::string binaryText;              //Given binaryText
    std::vector<int> binary;             //Make binary std::vector with 32 ints
    Field* info;                    //Input Field
    InstructionType type;

public:
    /* == Constructor ==
        Stores binary as text and integer, info, and Instructor type
        Compares binary's opcode to each Field's opcode (in decimal form).
    */
    Instruction(std::string Binary){
        binaryText = Binary;
        binary = processBinaryText();

        switch(getOpcode()){
            case(51):
                info = new RField(binary);
                type = R;
                break;
            case(19):
            case(3):
            case(103):
                info = new IField(binary);
                type = I;
                break;
            case(35):
                info = new SField(binary);
                type = S;
                break;
            case(99):
                info = new SBField(binary);
                type = SB;
                break;
            case(111):
                info = new UJField(binary);
                type = UJ;
                break;
            default:
                std::cout << "Not a valid use case!" << std::endl;
                break;
        }
        return;
    }

    /* == Constructor ==
        Stores values the same as given otherInstructor
    */
    Instruction(const Instruction& otherInstruction) : Instruction(otherInstruction.binaryText) {}

    /* == Destructor ==
        Deletes info pointer
    */
    ~Instruction(){
        delete info;
    }

    /* == printInfo ==
        Prints info about Instructor 
    */
    void printInfo(){
        info->printInfo();
    }

    /* == fieldData ==
        Returns info
    */
    std::map<std::string, int> fieldData(){
        return info->info();
    }

    /* == getType ==
        Returns Instructor type
    */
    InstructionType getType(){
        return type;
    }

    /* == isLW ==
        Returns if Instructor is LW
    */
    bool isLW(){
        return info->isLW();
    }

    /* == getALUOP ==
        Retrieves ALUOP
    */
    int getALUOP(){
        return info->getALUOP();
    }

private:
    /* == processBinaryText ==
        Convert binaryText to binary
    */
    std::vector<int> processBinaryText(){ 
        std::vector<int> ans;
        for(int i = 0; i < binaryText.length(); i++){
            ans.push_back((int)(binaryText[binaryText.length()-(i+1)])-48);
        }

        return ans;
    }

    /* == getOpcode ==
        Returns Opcode
    */
    int getOpcode(){                   
        int ans = binToInt(binary, {0, 1, 2, 3, 4, 5, 6});
        return ans;
    }

    /* == binToInt ==
        Convert the Opcode Binary to Hex
    */
    int binToInt(std::vector<int> binary, std::vector<int> indices = {}){
        int result = 0;
        if(indices.size() == 0){
            for(int i = 0; i < binary.size(); i++){
                result += binary[i] * pow(2,i);
            }
        }
        else{
            for(int i = 0; i < indices.size(); i++){
                result += binary[indices[i]] * pow(2,i);
            }
        }
        return result;
    }
};

#endif