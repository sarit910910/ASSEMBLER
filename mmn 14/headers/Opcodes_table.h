#ifndef OPCODE_TABLE_H
#define OPCODE_TABLE_H
#include "First_pass.h"
#define OPERAND_OPTIONS 5/* Covers 4 addressing methods and the option where no operand is present for this command */
#define OPCODES_LENGTH 4

/**
 * @brief Represents an instruction with its name, opcode, and operand addressing methods.
 * The `opcode` array contains:
 * - Indexes 0-3: Addressing methods for source and destination.
 * - Index 4: No operand.
 */
typedef struct {
    char* name;
    int opcode[OPCODES_LENGTH];
    int source_operand[OPERAND_OPTIONS];
    int destination_operand[OPERAND_OPTIONS];
} instruction;
/**
 * @brief Encodes the operation code based on the given index action.
 *
 * This function generates the operation code for a specified index action,
 * using the provided data structures.
 *
 * @param index_action An integer representing the index action (range 0-15) to encode.
 * @param locations A pointer to the structure containing IC and DC values.
 * @param structures A pointer to the structure with relevant data.
 */
void code_opcode(int index_action,DC_IC * locations,date_structures * structures);
/**
 * @brief Calculates the L value after verifying the argument count and types for the action.
 *
 * This function computes the L value if the number and types of arguments match the requirements
 * of the specified action.
 *
 * @param index_action An integer indicating the action index.
 * @param destination_operand_method An integer representing the addressing method for the destination operand.
 * @param source_operand_method An integer representing the addressing method for the source operand.
 *
 * @return The L value if all conditions are met; otherwise, returns a fictive value.
 */

int calculate_L_and_check(int index_action,int destination_operand_method,int source_operand_method);
/**
 * @brief Computes the word count required by an operand based on its addressing method.
 *
 * This function determines the number of words needed for the operand, given its addressing method.
 *
 * @param operand_method An integer specifying the operand's addressing method.
 *
 * @return An integer representing the calculated word count.
 */

int calculate(int operand_method);
/**
 * @brief Retrieves the action index corresponding to the specified action string.
 *
 * This function maps the provided action string to its corresponding index value.
 *
 * @param word A pointer to the action string to be evaluated.
 *
 * @return An integer representing the action index (0-15) associated with the string.
 */
int index_action(char * word);

 #endif  /* OPCODE_TABLE_H */ 
  
