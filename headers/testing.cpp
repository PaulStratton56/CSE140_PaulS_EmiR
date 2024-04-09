#include <vector>
#include <fstream>
#include <iostream>

using namespace std;


int main(){

    //vector<int> myvector = {1,2,3};

    std::ifstream MyFile;
    MyFile.open("../instructionFiles/sample_part1.txt");
    string line;

    if(MyFile.is_open()){
        while(getline(MyFile, line)){
            cout << line << endl;
        }
    }

    MyFile.close();
    

    return 0;
}