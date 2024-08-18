#ifndef SECOND_PASS_H
#define SECOND_PASS_H

#include "Symbols.h"
#include "Arrays.h"
#include "First_pass.h"
#include "Global_def.h"

/**
 * @brief Perform the second pass of the assembly process.
 *
 * This function handles the second pass of the assembly process. It processes
 * the encoded code and then encodes the labels and their addresses in the final
 * output. This includes writing the final machine code to the output files
 * and handling labels, symbols, and entries as needed.
 *
 * The function assumes that the first pass has already encoded the primary code
 * and sets up the necessary data structures. It processes the label definitions
 * and updates their addresses accordingly after the initial encoding has been
 * completed.
 *
 * @param structures A pointer to the data structure containing symbols, labels,
 *                   and other related data needed for the second pass.
 * @param locations A pointer to the data structure holding IC (Instruction Counter)
 *                  and DC (Data Counter) values.
 */

void second_pass(date_structures *structures, DC_IC *locations);

#endif /* SECOND_PASS_H */