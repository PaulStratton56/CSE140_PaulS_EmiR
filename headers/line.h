#ifndef LINE_H
#define LINE_H

#include <iostream>
#include <string>
#include <fstream>
#include <vector>



// class of lines. Read all the lines and store it in a pointer array?
/* == Line ==
    The same as a Node class.
    Stores a line from the textfile and points to the next line.
*/
class Line{
    int pc_num;
    std::string value;
    Line *next;

public:
    Line(std::string v){
        this->pc_num = 0;
        value = v;
        next = NULL;
    }

    Line(int pc_num, std::string value){
        this->pc_num = pc_num;
        this->value = value;
        next = NULL;
    }

    std::string getValue(){
        return value;
    }

    void setNext(Line *n){
        next = n;
    }

    Line getNext(){
        return *next;
    }

    friend class Simulation;
};

// /* == PC ==
//     PC has access to all the Lines.
//     The constructor stores all the binary instructions into Line classes which point to the next Line. 
//     fetch(): returns current binary instruction
//     nextPC(): points to the next Line
// */
// class PC{
//     Line *current;
//     Line *start;
//     int pc = 0;
//     int* next_pc = &pc +4;

// public:
//     PC(){
//         std::ifstream MyFile("file.txt");
//         std::string line;

//         if(getline(MyFile, line)){
//             current = new Line(line);
//         }

//         start = current;

//         while (getline(MyFile, line)) { 
//             current->next = new Line(line);
//             current = current->next;
//         } 

//         current = start;
//     }

//     PC(std::string textfile){
//         std::ifstream MyFile(textfile);
//         std::string line;

//         if(getline(MyFile, line)){
//             current = new Line(line);
//         }

//         start = current;

//         while (getline(MyFile, line)) { 
//             current->next = new Line(line);
//             current = current->next;
//         } 

//         current = start;
//     }

//     std::string fetch(int pc_num){
//         return current->getValue();
//     }
// };

#endif