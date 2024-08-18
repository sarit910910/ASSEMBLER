#ifndef FIRST_PASSS_H
#define FIRST_PASSS_H
#include "NewMacro.h"
#include "Global_def.h"
#include "Arrays.h"


typedef struct {
    struct macro_linked_list * macro_list;
    struct symbols_linked_list * symbols_list;
    bit_field * data_array;
    bit_field * instructions_array;
    LabelNode * label_list;
} date_structures;

typedef struct {
    int external_error_exist;
    int internal_error_exist;
}errors_status;

typedef struct {
    int DC;
    int IC;
}DC_IC;


// פונקציה לאתחול המערך
//Label * initLabels(size_t initialSize);

// פונקציה להוספת איבר למערך
//Label* addLabel(Label *labels,  int emptyLine, const char *labelName);

//nt getSize(Label *labels);//DEBUG

/**
 * Perform the first pass.
 *
 * @param list A pointer to the macro-linked list.
 * @param error_exist A pointer to an integer where error existence status will be stored.
 * @param file A pointer to the file status structure.
 * @param file_am A pointer to the FILE object representing the file to be processed.
 *
 * @return Returns an integer indicating the success or failure of the operation.
 *         -  NO_ERROER if the operation was successful.
 *         - INTERNAL_ERROR if an internal error occurred during execution.
 */
int first_pass_main(struct macro_linked_list * list, int * error_exist,struct file_status * file,FILE * file_am);
/**
 * Send the line to the suitable function.
 *
 * @param is_label An integer flag indicating whether the line begun with label: or not.
 * @param ptp A pointer to a pointer to the line.
 * @param file A pointer to the file status structure.
 * @param errors A pointer to the errors status structure.
 * @param locations A pointer to the data structure holding IC and DC.
 * @param structures A pointer to the data structures.
 *
 * @return Returns a pointer to a symbols_node structure containing processed data.
 *         Returns NULL if an error occurred during processing.
 */
struct symbols_node* send_to_function(int is_label,char ** ptp,struct file_status * file, errors_status * errors,DC_IC * locations,date_structures * structures );
/**
 * treats line that holds data.
 *
 * @param ptp A pointer to a pointer to the line.
 * @param file A pointer to the file status structure.
 * @param errors A pointer to the errors status structure.
 * @param locations A pointer to the data structure holding IC and DC.
 * @param structures A pointer to the data structures.
 *
 * @return Returns a pointer to a symbols_node structure containing processed data(for case that this line *  begun with label:).
 *         Returns NULL if an error occurred during processing.
 */
struct symbols_node* data(char ** ptp ,struct file_status * file, errors_status * errors,DC_IC * locations,date_structures * structures );
/**
 * treats line that holds string.
 *
 * @param ptp A pointer to a pointer to the line.
 * @param file A pointer to the file status structure.
 * @param errors A pointer to the errors status structure.
 * @param locations A pointer to the data structure holding IC and DC.
 * @param structures A pointer to the data structures.
 *
 * @return Returns a pointer to a symbols_node structure containing processed data.for case that this line *  begun with label:).
 *         Returns NULL if an error occurred during processing.
 */
struct symbols_node* string(char ** ptp ,struct file_status * file, errors_status * errors,DC_IC * locations,date_structures * structures );
/**
 * insert data to the data array.
 *
 * @param value value to enter to the array.
 * @param file A pointer to the file status structure.
 * @param errors A pointer to the errors status structure.
 * @param locations A pointer to the data structure holding IC and DC.
 * @param structures A pointer to the data structures.
 */
void insert_data(int value,  struct file_status * file, errors_status * errors,DC_IC * locations,date_structures * structures );
/**
 * treats line that begins with label decleration.
 * 
 * @param label The name of the label.
 * @param ptp A pointer to a pointer to the line.
 * @param file A pointer to the file status structure.
 * @param errors A pointer to the errors status structure.
 * @param locations A pointer to the data structure holding IC and DC.
 * @param structures A pointer to the data structures.
 *
 * @return Returns a pointer to a symbols_node structure .it will be NULL anyway.
 */
struct symbols_node* label(char * label,char ** ptp ,struct file_status * file, errors_status * errors,DC_IC * locations,date_structures * structures );
/**
 * treats line that begins with extern:.
 * 
 * @param ptp A pointer to a pointer to the line.
 * @param file A pointer to the file status structure.
 * @param errors A pointer to the errors status structure.
 * @param locations A pointer to the data structure holding IC and DC.
 * @param structures A pointer to the data structures.
 *
 * @return Returns a pointer to a symbols_node structure.it will be NULL anyway.(coz we skip label that holds extern decleration)
 */
struct symbols_node* extern_line(char ** ptp ,struct file_status * file, errors_status * errors,DC_IC * locations,date_structures * structures );

/**
 * treats line that begins with action.
 * 
 * @param index_of_action the index of this action(0-15)
 * @param ptp A pointer to a pointer to the line.
 * @param file A pointer to the file status structure.
 * @param errors A pointer to the errors status structure.
 * @param locations A pointer to the data structure holding IC and DC.
 * @param structures A pointer to the data structures.
 *
 * @return Returns a pointer to a symbols_node structure.
 */
struct symbols_node* action(int index_of_action,char ** ptp ,struct file_status * file, errors_status * errors,DC_IC * locations,date_structures * structures );
/**
 * code the method of the operand to the first word.
 * 
 * @param first_bit the location of the first bit )from left) to be code.
 * @param opernad_method the method that suitable this operand.
 * @param locations A pointer to the data structure holding IC and DC.
 * @param structures A pointer to the data structures.
 * @param list A symbol list to check whether external or internal according to the direct address method.
 * @param oparand A pointer to have a pointer to the operand to be parsed.
 */
void code_operand_method(int first_bit,int opernad_method,DC_IC * locations,date_structures * structures,struct symbols_linked_list * list, char * opernad);
/**
 * adds values to the symbols list as needed (at the end of the first pass).
 *
 * @param locations A pointer to the data structure holding IC and DC.
 * @param structures A pointer to the data structures.
 */
void end_of_pass_1(DC_IC * locations,date_structures * structures,struct file_status * file,FILE * file_am);


void printBinary15Bit(int n);//DEBUG

void entries(struct file_status * file,FILE * file_am,  date_structures * structures ,DC_IC * locations);
struct symbols_node* send_to_function_pass_two(char ** ptp,struct file_status * file, date_structures * structures);

int compare_strings_ignore_special_chars(const char *str1, const char *str2);

int compare_strings(const char *str1, const char *str2);
#endif /* FIRST_PASS_H */
