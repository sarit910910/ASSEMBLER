#ifndef GLOBAL_FUN_H
#define GLOBAL_FUN_H
#include "NewMacro.h"
#include "Global_def.h"
#include "Symbols.h"
#include "First_pass.h"

/**
 * @brief Creates a copy of a string.
 *
 * This function allocates memory for a new string and copies the contents of the
 * provided source string into the newly allocated memory.
 *
 * @param src Pointer to the string to be copied.
 * 
 * @return A pointer to the newly allocated string copy. Returns NULL if memory allocation fails.
 */

char *my_strdup(char *src);


/**
 * @brief Verifies if the rest of a line consists solely of whitespace characters.
 *
 * @param line The input string to be checked.
 * @param index The position in the string from which to start the check.
 * @param file_status The name of the file and the current line number for reference.
 * @param error_exists A pointer to a variable indicating whether an error has been detected in the file.
 * 
 * @return Returns 1 if only whitespace characters are found in the remaining part of the line, 0 otherwise.
 */

int valid_end(char *line, int index, struct file_status* file, int *error_exists);

/**
 * @brief Validates whether a given word is legal. A legal word must start with 
 * an alphabetic character, contain only printable characters, and must not 
 * match any reserved words in the array of reserved words.
 *
 * @param str The word to validate.
 * @param is_define Indicates if the word is being checked as a defined symbol or not.
 * @param list A pointer to the list of macros.
 * @param file_status The name of the file and the line number for reference.
 * 
 * @return Returns 1 if the word is legal, 0 if it is not.
 */

int legal_word( char *str,int is_define, struct macro_linked_list *list, struct file_status* file);

/**
 * Populate a buffer and verify end-of-file status.
 *
 * @param buffer A pointer to the buffer where the line will be stored.
 * @param file A pointer to the FILE object representing the file to read from.
 *
 * @return Returns an integer indicating whether the end of the file has been reached.
 */

void free_strings(int num_strings, ...);
/**
 * Fill a buffer and check for end of file.
 *
 * @param buffer A pointer to the buffer where the line will be stored.
 * @param file A pointer to the FILE object representing the file to read from.
 *
 * @return Returns an integer indicating whether there is EOF or not.
 *       
 */
int fill_and_check(char * buffer,FILE * file);
/**
 * Bypass leading spaces in a string.
 *
 * @param ptp A pointer to a pointer to the string where the spaces will be bypassed.
 *
 * @return Returns an integer indicating whether there are spaces until the end of the line or not.
 */

int skip_spaces(char ** ptp);
/**
 * Extract the next word from a string.
 * This function assumes that there are no spaces at the beginning of the line.
 * It retrieves the next word from a string and moves the pointer forward to the subsequent word.
 *
 * @param ptp A pointer to a pointer to the string.
 *
 * @return Returns a pointer to the next word in the string.
 */

char * next_word(char ** ptp);
/**
 * Get the next parameter from a string (stops in comma).
 * This function assume that there is no spaces in the begining of the line
 * This function extracts the next word from a string and advances the pointer to the next parameter.
 *
 * @param ptp A pointer to a pointer to the string.
 *
 * @return Returns a pointer to the next parameter in the string.
 */
char * next_param(char ** ptp);
/**
 * Identify the addressing mode of a given operand.
 *
 * This function examines a specified operand to determine which addressing mode it uses.
 * It references a linked list of symbols to assist in the analysis.
 *
 * @param operand A pointer to the operand that needs to be evaluated.
 * @param list A pointer to the linked list of symbols, which is used as a reference during the evaluation.
 *
 * @return Returns an integer indicating the addressing mode, based on the definitions in Global_def.h.
 */

int figure_addressing_methods(char * opernad,struct symbols_linked_list * list);

 /** Convert a decimal number to its binary representation with a specified number of bits.
 * This function takes an integer `n` and converts it to a binary number represented as an array of integers,
 * with each element being either 0 or 1. The result is stored in a dynamically allocated array of size `bits`.
 * 
 * @param n The decimal number to be converted. It can be a positive or negative integer.
 * @param bits The number of bits in the binary representation. The function assumes that `bits` is large enough to represent the number.
 * 
 * @return Returns a pointer to an array of integers representing the binary number. 
 *         The array must be freed by the caller to avoid memory leaks.
 */
int* decimalToBinary(int n, int bits);
#endif  /* GLOBAL_FUN_H */
