#include "headers/simulation.h"

using namespace std;

int main(){

    string instructionFilePath;
    cout << "Enter the program file name to run:" << endl << endl;
    cin >> instructionFilePath;
    cout << endl;

    Simulation simulation = Simulation(instructionFilePath);
    simulation.run();

    return 0;
}