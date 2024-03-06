#ifndef MEMORY_H
#define MEMORY_H

#include "registers.h"

class Memory{
protected:
    int* data;
    int size;

    virtual void setData(int index, int value) = 0;
    virtual int getData(int index) = 0;

    Memory(int size){
        this->size = size;
        data = new int[this->size];
        for(int i = 0; i < this->size; i++){
            data[i] = 0;
        }
    }

    ~Memory(){
        delete[] data;
    }
};

class DataMemory : public Memory{
public:
    DataMemory(int size) : Memory(size){}

    void setData(int index, int value){
        this->data[index] = value;
    }

    int getData(int index){
        return this->data[index];
    }

    ~DataMemory(){}

};

class RegisterFile : public Memory{
public:
    RegisterFile(int size) : Memory(size){}

    void setData(int index, int value){
        this->data[index] = value;
    }

    void setData(Register reg, int value){
        this->data[reg] = value;
    }

    int getData(int index){
        return this->data[index];
    }

    ~RegisterFile(){}

};


#endif