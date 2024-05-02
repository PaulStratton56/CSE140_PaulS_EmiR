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

#endif