#include "headers/simulation.h"

using namespace std;

int main(){

    string instructionFilePath;
    cout << "Enter filepath: ";
    cin >> instructionFilePath;
    cout << "Running " << instructionFilePath << "..." << endl;

    Simulation simulation = Simulation(instructionFilePath);
    simulation.run();

    return 0;
}