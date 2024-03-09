#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <cmath>
#include "fields.h"

class Instruction{
public:
    Instruction(std::string Binary){     //Constructor
        binaryText = Binary;
        binary = processBinaryText();

        /* Compares binary's opcode to each Field's opcode (in decimal form). */
        switch(getOpcode()){
            case(51):
                info = new RField(binary);
                break;
            case(19):
            case(103):
                info = new IField(binary);
                break;
            case(35):
                info = new SField(binary);
                break;
            case(99):
                info = new SBField(binary);
                break;
            case(111):
                info = new UJField(binary);
                break;
            default:
                std::cout << "Not a valid use case!" << std::endl;
                break;
        }
        return;
    }

    ~Instruction(){                 //Destructor
        delete info;
    }

    void printInfo(){               //Printing Information
        info->printInfo();
    }

private:
    std::string binaryText;              //Given binaryText
    std::vector<int> binary;             //Make binary std::vector with 32 ints
    Field* info;                    //Input Field

    std::vector<int> processBinaryText(){       //Convert binaryText to binary
        std::vector<int> ans;
        for(int i = 0; i < binaryText.length(); i++){
            ans.push_back((int)(binaryText[binaryText.length()-(i+1)])-48);
        }

        return ans;
    }

    int getOpcode(){                   //Convert the Opcode Binary to Hex
        int ans = binToInt(binary, {0, 1, 2, 3, 4, 5, 6});
        return ans;
    }

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

    std::string type;    //Type of Field
};

#endif