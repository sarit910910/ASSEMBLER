#ifndef ERROR_H
#define ERROR_H
#include "Global_def.h"

/*זה לא מעודכן, זה רק בשביל ההרצה*/


typedef enum ERROR_CODES{
    /* each one represent index of the array "errors" */
    w_double_define = 0,
    w_double_extern,
    w_double_entry,
	w_define_decleration_double ,
	w_extern_double ,
	
	double_label_define_value,
	missing_param_after_comma,
	double_label,
	entry_un_realized,
	 double_external_entry,
	double_define_entry,
	wrong_param_fun,
	double_define_name,
	invalid_num,
	missing_assignment_sign,
	missing_define_value,
	empty_define,
	empty_extern,
	empty_label,
	missing_ending_char,
	invalid_char_in_string,
	string_with_invalid_begin,
	no_such_define,
	missing_parameter,
	unrocognize_line,
	no_such_file,
     no_files,
    remove_fail,
    memory_failed ,
    file_not_exist,
    too_long_line,
    write_fail,
    read_fail,
    extra_text,
    macro_name_missing,
    double_name_macro,
    illegal_name_macro,
	too_big_number,
	missing_comma,
    not_label,
    illegal_entry,
    illegal_extern,
    too_commas,
    illegal_comma,
    illegal_name_label,
    missing_colon_label,
    extra_text_end,
    double_name_label,
    double_name_label_extern,
    illegal_name,
    reserved_word,
    defined_macro,
large_value,
Invalid_used_label,
invalid_immediate,
missing_bracket,
space_before_bracket,
invalid_index,
index_not_exists,
illegal_char,
empty_entry,
invalid_operand,
 operand_not_correspond,
too_many_words
} ERROR_CODES;



/**
 * @brief Prints an external error message
 *
 * This function prints to stdout an error that is related to an error in the source file
 * with the error location in the source file
 * @param error error number
 * @param file_status the name of the file and the line where the error
 */
void print_external_error(int error, struct file_status*file);

/**
 * @brief Prints an internal error message.
 *
 * This function prints to stdout an error that is a result of an internal process and NOT an error in the source file
 * @param error error 
 */
void print_internal_error(int error);

/**
 * @brief Prints an warning message.
 *
 * This function prints to stdout warning with the warning location in the source file
 * @param warning warning number
 * @param file_status the name of the file and the line where the error
 */
void print_warning(int warning, struct file_status*file);

/**
 * @brief Prints a custom error message.
 *
 * This function prints to stdout a custom error message with the error location in the source file.
 * @param message The error message to print.
 * @param line The line number where the error occurred.
 */
void print_error(const char *message, int line);
#endif //ERRORS_H
