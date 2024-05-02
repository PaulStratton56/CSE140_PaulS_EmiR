#ifndef MEMORY_H
#define MEMORY_H

#include "registers.h"

/* == Memory ==
    Abstract class for Memory 
    Stores data and size
*/
class Memory{
protected:
    int* data;
    int size;

    virtual void setData(int index, int value) = 0;
    virtual int getData(int index) = 0;

    /* == Constructor ==
        Stores size and sets each value in data to 0
    */
    Memory(int size){
        this->size = size;
        data = new int[this->size];
        for(int i = 0; i < this->size; i++){
            data[i] = 0;
        }
    }

    /* == Deconstructor ==
        Deletes data array
    */
    ~Memory(){
        delete[] data;
    }
};

/* == DataMemory ==
    Child class of Memory
    Same at Memory except meant for Data Memory
*/
class DataMemory : public Memory{
public:
    /* == Constructor == */
    DataMemory(int size) : Memory(size){}

    /* == setData ==
        Set value at specified index
    */
    void setData(int index, int value){
        this->data[index] = value;
    }

    /* == getData ==
        Recieve data at specified index
    */
    int getData(int index){
        return this->data[index];
    }

    /* == Destructor == */
    ~DataMemory(){}

};

/* == RegisterFile ==
    Child class of Memory
    Same at Memory except meant for Register File Memory
*/
class RegisterFile : public Memory{
public:
    /* == Constructor == */
    RegisterFile(int size) : Memory(size){}

    /* == setData ==
        Set value at specified index
    */
    void setData(int index, int value){
        this->data[index] = value;
    }

    /* == setData ==
        Set value at specified Register
    */
    void setData(Register reg, int value){
        this->data[reg] = value;
    }

    /* == getData ==
        Recieve data at specified index
    */
    int getData(int index){
        return this->data[index];
    }

    /* == Destructor == */
    ~RegisterFile(){}

};


#endif