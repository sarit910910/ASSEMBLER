#ifndef NEW_MACRO_H
#define NEW_MACRO_H

#include "Errors.h"
#include "Global_def.h"
#include <stdio.h>

#define MAX_LINE_LENGTH 82
#define MAX_MACRO_NAME_LENGTH 31

#define MACRO_NOT_FOUND 4
#define RESERVED_WORD_ERROR 5

typedef struct macro_node {
    char *name;
    char *content;
    struct macro_node *next;
} MacroNode;

typedef struct macro_linked_list {
    MacroNode *head;
    MacroNode *tail;
} MacroList;

typedef enum {
    MACRO_DEF,
    MACRO_END_DEF,
    MACRO_CALL,
    COMMENT_LINE,
    ANY_OTHER_LINE_TYPE,
    DATA_LINE
} LineType;

/**
 * @brief Creates a new macro list.
 *
 * This function allocates memory for a new `MacroList` structure and initializes it.
 * The newly created macro list is ready to be populated with macro definitions.
 *
 */

MacroList* create_macro_list();
/**
 * @brief Inserts a new macro into the macro list.
 *
 * This function adds a new macro definition to the provided macro list. It stores the
 * macro's name and content, and logs information about the file where the macro is defined.
 *
 * @param list A pointer to the `MacroList` where the new macro will be inserted.
 * @param name A pointer to a string containing the name of the macro.
 * @param content A pointer to a string containing the content of the macro.
 * @param file A pointer to a `file_status` structure providing information about the file and line number.
 * 
 * @return Returns 1 if the macro was successfully inserted into the list, 0 otherwise.
 */

int insert_new_macro(MacroList *list, const char *name, const char *content, struct file_status *file);
/**
 * @brief Executes a macro by name and writes its content to an output file.
 *
 * This function searches for a macro by its name in the given MacroList. 
 * If the macro is found, its content is written to the specified output file.
 * If the macro is not found, an error is reported indicating that the macro is not defined.
 *
 * @param name The name of the macro to be executed.
 * @param output_file A pointer to the file where the macro content will be written.
 * @param list A pointer to the MacroList containing all defined macros.
 * @param file A pointer to the file_status structure for error reporting.
 */

void call_macro(const char *name, FILE *output_file, MacroList *list, struct file_status *file);
/**
 * @brief Frees all memory associated with a MacroList.
 *
 * This function deallocates all memory used by the macros in the list and the list itself. 
 * It iterates through the linked list of macros, freeing the memory for each node and 
 * its associated content.
 *
 * @param list A pointer to the MacroList to be freed. If the list is NULL, the function does nothing.
 */

void free_macro_list(MacroList *list);
/**
 * @brief Checks if a given word is a reserved word.
 *
 * This function determines whether the provided word matches any of the reserved words 
 * in the assembler's predefined list. Reserved words are typically keywords or 
 * special identifiers that cannot be used as labels or variable names.
 *
 * @param word A pointer to the string representing the word to check.
 *
 * @return Returns 1 if the word is a reserved word, 0 if it is not.
 */

int is_reserved_word(const char *word);  
/**
 * @brief Main function for preprocessing the input assembly file.
 *
 * This function handles the preprocessing of the input assembly file, including
 * macro expansion and any other preprocessing directives. The function processes
 * the input file line by line, expanding macros and writing the processed content 
 * to an output file.
 *
 * @param error_exist A pointer to an integer that indicates if an error was encountered during preprocessing.
 * @param file A pointer to a struct containing the status of the file, such as the filename and line number.
 * @param file_as A pointer to the input assembly file (typically with a .as extension).
 * @param file_am A pointer to the output file where the preprocessed content will be written (typically with a .am extension).
 * @param list A pointer to the macro list used during preprocessing.
 *
 * @return Returns 0 if preprocessing was successful, or a non-zero value if an error occurred.
 */

int pre_proccesor_main(int *error_exist, struct file_status* file, FILE *file_as, FILE *file_am, MacroList *list);
/**
 * @brief Generates the output filename based on the input filename.
 *
 * This function takes an input filename, removes its extension if present, 
 * and generates a corresponding output filename by appending an appropriate 
 * extension or suffix.
 *
 * @param input_filename A pointer to the input filename string.
 * @param output_filename A pointer to a buffer where the generated output filename will be stored.
 */

void create_output_filename(char *input_filename, char *output_filename);
/**
 * @brief Checks if a macro name has already been defined in the macro list.
 *
 * This function searches through the macro list to determine if a given macro name
 * has already been defined.
 *
 * @param list A pointer to the MacroList containing the defined macros.
 * @param str The macro name to check.
 *
 * @return 1 if the macro name is found in the list, 0 if it is not found.
 */

int macro_name_appeared(MacroList *list, const char *str);
/**
 * @brief Determines the type of a given line in the source file.
 *
 * This function analyzes a line of code to determine its type, such as whether it is a macro definition, 
 * a directive, or a regular instruction. If the line is a macro definition, the macro name is extracted.
 *
 * @param line A pointer to the line of text to be analyzed.
 * @param list A pointer to the MacroList containing existing macros for reference.
 * @param macro_name A double pointer to store the name of the macro if the line is a macro definition.
 *
 * @return The type of the line as a `LineType` enum value.
 */

LineType determine_line_type(const char *line, MacroList *list, char **macro_name);
/**
 * @brief Processes an array of data lines and writes the processed output to a file.
 *
 * This function takes an array of data lines, processes each line according to the required format or logic,
 * and writes the processed data to the specified output file.
 *
 * @param data_lines An array of strings, each representing a data line to be processed.
 * @param num_data_lines The number of data lines in the array.
 * @param file_am A pointer to the output file where the processed data will be written.
 */

void process_data_lines(char *data_lines[], int num_data_lines, FILE *file_am);
#endif