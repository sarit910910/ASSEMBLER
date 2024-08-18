#include "Errors.h"
#include "Global_def.h"
#include "Global_fun.h"
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

/* זה לא מעודכן, זה רק בשביל ההרצה */

/*The array representing errors in files*/
char * errors[85] = {

          "This constant has been defined twice",
          "This label has already been declared as extern",
          "This label has already been declared as entry",
	"This constant with the same value was already declared",
	 "This label already declared as extern",
	"There is already constant that holds another value,using the same name",
	"There is no argument after the comma",
	"There is already another variable using this name",
	"This label was never realized,can declared as entry",
	"Extern type variable cannot be defined as an entry",
	"A constant type variable cannot be defined as an entry",
        "This action gots wrong number of parameters here, or not suitable parameter ",
	  "There is label using this name, or define that using this name but with another value",
	"There is no valid number",
	"Missing assisnment sign in define",
	"Missing value to define",
	"There is no name of the define",
	"There is no parameter after extern",
	"Empty label",
	"Missing closing apostrophes for the string",
	"The string contains a non-printable character",
	"The string does not start with quotation marks",
	"A constant with this name has not yet been defined",
	"There is missing parameter in this line",
	"This is un-recognized line.you might have problems with the syntax",
	"There is no file in this name",
		"There is no files to open",
           "Failed to remove file",
          "Failed to dynamically allocate memory",
         "The provided file name is not exist",
         "The line is too long",
         "Failed to open new file for writing",
         "Failed to open file for reading",
         "Extra text",
          "The name of the macro has not been defined (yet)",
         "A macro with that name already exists",
         "Illegal name for a macro",
	"There is too big number",
	"Missing comma",
         "Label not defined in the assembly file",
         "Illegal label after .entry",
          "Illegal label after .extern",
         "More commas than needed",
         "Comma in the wrong place",
         "Illegal label declaration",
         "Missing ':' after label declaration",
        "Extra text after the string end in '.string' line",
       "Label definition repeats more than once",
       "Label defined as .extern and defined in file",
        "illegal name",
        "This is a reserved word",
        "This symbol has been defined as a macro",
"This number cannot be represented due to space limitations",
	"Writing the label n this form is illegal",
	"Setting up a immediate in this way is illegal",
	"Opening bracket missing",
	"Space detected before opening bracket",
	"Non-digit character in index and it isn't define",
	"This index does not exist in the array",
	"Invalid character in name definition",
	"A statement about an entry without a label",
	"There is no matching sort method for this operand",
	"This operands does not correspond to the possible addressing method of the operation",
	"The number of words in this file is greater than the allowed number of words"
};

void print_internal_error(int error) {
    /* Print the error message */
    printf("~~ERROR:~~ | %s\n",  errors[error]);
}


void print_warning(int warning,struct file_status * file) {
   /* Print the file name, assembly line number and the warning message */
    printf("~~Warning:~~ in %s at line:%d | %s\n", 
     file->name, file->line, errors[warning]);
}


void print_external_error(int error, struct file_status * file) {
    /* Print the file name, assembly line number and the error message */
    printf("~~ERROR:~~ in %s at line:%d | there is error: %s\n", 
     file->name, file->line, errors[error]);
}

void print_error(const char *message, int line) {
    fprintf(stderr, "Error on line %d: %s\n", line, message);
}
