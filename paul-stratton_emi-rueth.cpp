#include <iostream>
#include "instruction.h"

using namespace std;

int main(){

    string binary;

    cout << "Enter an instruction: ";
    cin >> binary;
    
    Instruction myInstruction(binary);
    myInstruction.printInfo();

    return 0;
}