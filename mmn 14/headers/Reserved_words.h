/*This file contains a declaration of an array containing the reserved words in the language*/
#ifndef RESERVED_WORD_H
#define RESERVED_WORD_H
#define RESERVED_WORD_NUM 31
/*An array of all reserved words in the language*/
extern char strings[31][10];
/**
 * @brief Checks if a given string exists in the array of reserved words.
 *
 * @param str The string to check for existence in the array.
 * 
 * @return 1 if the string exists in the array, 0 otherwise.
 */
int reserved_words( char *str);
/**
 * Check if a string represents a register.
 *
 * This function checks if the given string represents a register.
 *
 * @param str A pointer to the string to be checked.
 *
 * @return Returns an integer indicating whether the string represents a register:
 *         - TRUE if the string represents a register.
 *         - FALSE if the string does not represent a register.
 */
int is_register(char * str);
#endif
