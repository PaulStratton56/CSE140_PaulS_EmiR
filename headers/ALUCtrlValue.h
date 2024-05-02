#ifndef ALUCTRLVALUE_H
#define ALUCTRLVALUE_H

/* == ALUCtrlValue ==
    An enum that sets ALUCtrl will number values
*/
enum ALUCtrlValue{
    NONE = -1,
    AND = 0,
    OR  = 1,
    ADD = 2,
    SUB = 6,
    //These ones I made up for I-Type.
    SLL = 10,
    SRL = 11,
    SRA = 12,
    SLT = 4,
    XOR = 5,
};

#endif