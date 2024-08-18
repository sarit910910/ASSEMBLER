#ifndef MY_ARREYS_H
#define MY_ARREYS_H

#include <stdio.h>
#include <stdlib.h>


#define BIT_FIELD_SIZE 15

/* Structure representing a 14-bit bitfield*/
typedef struct {
    unsigned int data : BIT_FIELD_SIZE;
} bit_field;

typedef struct LabelNode {
    char *label;  
    int address;  
    struct LabelNode *next;  
} LabelNode;


/**
 * Set a specific bit in a bitfield.
 *
 * @param bitfield Pointer to the bitfield structure.
 * @param pos      Position of the bit to set (0-indexed).
 * @param value    Value to set the bit to (0 or 1).
 * @return         None.
 */
void set_bit(bit_field *bitfield, int pos, int value);


LabelNode* createLabelNode(const char *label, int address);
void addLabelNode(LabelNode **head, const char *label, int address);
void freeLabelList(LabelNode *head);
void printLabelList(LabelNode *head);


#endif /* MY_ARREYS_H */
