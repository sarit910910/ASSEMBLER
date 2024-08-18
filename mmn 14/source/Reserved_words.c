#include "Reserved_words.h"
#include "Global_def.h"
#include <string.h>

/*An array of all reserved words in the language*/
char strings[RESERVED_WORD_NUM][10] = {
    /*directive names*/
    ".data",
    ".string",
    ".entry",
    ".extern",
    /*Used to define a macro*/
    "macr",
    "endmacr",
    /*16 instructions in our assembly language*/
    "mov",
    "cmp",
    "add",
    "sub",
    "lea",
    "clr",
    "not",
    "inc",
    "dec",
    "jmp",
    "bne",
    "red", 
    "prn",
    "jsr", 
    "rts",
    "stop",
    /*These are the 8 registers of the processor*/
    "r0",
    "r1",
    "r2",
    "r3",
    "r4",
    "r5",
    "r6",
    "r7"
};

//בודקת אם קיימת מילה במאגר סטרינגס
int reserved_words( char *str) {
    int i;
    for (i = 0; i <RESERVED_WORD_NUM; i++) {
        if (strcmp(strings[i], str) == 0)
            return TRUE; /* String exists in the array*/
    }
    return FALSE; /* String does not exist in the array*/
}

//בודקת אם קיים שם של רגיסטר
int is_register(char * str){
	/*where the registers begins*/
	int register_index=22;
	int i;
    	for (i = register_index; i < RESERVED_WORD_NUM; i++) {
        	if (strcmp(strings[i], str) == 0)
            	return TRUE; /* String is register name*/
    	}
    	return FALSE; /* String does not register name*/
}
