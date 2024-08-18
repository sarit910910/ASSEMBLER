#include "NewMacro.h"
#include "Errors.h"
#include "Reserved_words.h"
#include "Global_fun.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_DATA_LINES 10//DEBUG

// Function to create a new macro list
MacroList* create_macro_list() {
    MacroList *list = (MacroList *)malloc(sizeof(MacroList));
    if (!list) {
        print_internal_error(memory_failed);
        return NULL;
    }
    list->head = list->tail = NULL;
    return list;
}

int insert_new_macro(MacroList *list, const char *name, const char *content, struct file_status *file) {
    if (strlen(name) > MAX_MACRO_NAME_LENGTH) {  // שימוש בקבוע להגבלת אורך השם
        print_error("Error: Macro name is too long (more than 31 characters)", file->line);
        return RESERVED_WORD_ERROR;
    }

    if (is_reserved_word(name)) {
        print_error("Error: Cannot use reserved word as macro name", file->line);
        return RESERVED_WORD_ERROR;
    }

    if (macro_name_appeared(list, name)) {
        print_error("Error: Macro name already exists", file->line);
        return RESERVED_WORD_ERROR;
    }

    MacroNode *new_macro = (MacroNode *)malloc(sizeof(MacroNode));
    if (!new_macro) {
        print_internal_error(memory_failed);
        return memory_failed;
    }
    new_macro->name = strdup(name);
    new_macro->content = strdup(content);
    if (!new_macro->name || !new_macro->content) {
        print_internal_error(memory_failed);
        free(new_macro->name);
        free(new_macro->content);
        free(new_macro);
        return memory_failed;
    }
    new_macro->next = NULL;
    if (!list->head) {
        list->head = list->tail = new_macro;
    } else {
        list->tail->next = new_macro;
        list->tail = new_macro;
    }
    return 0;
}



// Function to call a macro and replace it with its content
void call_macro(const char *name, FILE *output_file, MacroList *list, struct file_status *file) {
    MacroNode *current = list->head;
    while (current) {
        if (strcmp(current->name, name) == 0) {
            fprintf(output_file, "%s", current->content);
            return;
        }
        current = current->next;
    }
    print_error("Error: Macro not defined", file->line);
}


// Function to free the memory used by the macro list
void free_macro_list(MacroList *list) {
    MacroNode *current = list->head;
    while (current) {
        MacroNode *next = current->next;
        free(current->name);
        free(current->content);
        free(current);
        current = next;
    }
    free(list);
}

// Function to determine the type of a line
// עדכון הפונקציה determine_line_type לזיהוי שורת הערה
LineType determine_line_type(const char *line, MacroList *list, char **macro_name) {
    // בדיקה אם השורה מתחילה בתו ';' והיא שורת הערה
    if (line[0] == ';') {
        return COMMENT_LINE;  // החזרת סוג שורה חדש לשורת הערה
    }

    // בדיקה אם השורה מתחילה ב".data"
    if (strncmp(line, ".data", 5) == 0) {
        // החזרת סוג שורה חדש עבור שורת .data
        return DATA_LINE;
    } 

    if (strncmp(line, "macr", 4) == 0) {
        *macro_name = strdup(line + 5);
        char *newline_pos = strchr(*macro_name, '\n');
        if (newline_pos != NULL) {
            *newline_pos = '\0';
        }
        // בדיקה אם יש תווים נוספים אחרי שם המקרו
        char *extra_chars = strchr(*macro_name, ' ');
        if (extra_chars != NULL && strlen(extra_chars) > 1) {
            return RESERVED_WORD_ERROR;
        }
        return MACRO_DEF;
    }

    if (strncmp(line, "endmacr", 7) == 0) {
        if (strlen(line) > 7 && line[7] != '\n' && line[7] != '\0' && strchr(line + 7, ' ') != NULL) {
            return RESERVED_WORD_ERROR;
        }
        return MACRO_END_DEF;
    }

    MacroNode *current = list->head;
    while (current) {
        if (strncmp(line, current->name, strlen(current->name)) == 0) {
            *macro_name = strdup(current->name);
            return MACRO_CALL;
        }
        current = current->next;
    }

    return ANY_OTHER_LINE_TYPE;
}

// עדכון הפונקציה pre_proccesor_main כדי לדלג על שורות הערה
int pre_proccesor_main(int *error_exist, struct file_status* file, FILE *file_as, FILE *file_am, MacroList *list) {
    char line[MAX_LINE_LENGTH + 1];
    char macro_content[MAX_LINE_LENGTH + 1];
    char *macro_name = NULL;
    LineType line_type;
    int macro_open = 0;

    char *data_lines[MAX_DATA_LINES];//DEBUG
    int num_data_lines = 0;//DEBUG

    while (fgets(line, sizeof(line), file_as)) {
        file->line++;

        // זיהוי סוג השורה
        line_type = determine_line_type(line, list, &macro_name);


        if (line_type == DATA_LINE) {//DEBUG
            if (num_data_lines >= MAX_DATA_LINES) {
                print_error("Error: Too many .data lines", file->line);
                *error_exist = 1;
                continue;
            }
            data_lines[num_data_lines] = strdup(line);
            if (!data_lines[num_data_lines]) {
                print_internal_error(memory_failed);
                *error_exist = 1;
                continue;
            }
            num_data_lines++;
            continue;
        }


        // דילוג על שורות הערה
        if (line_type == COMMENT_LINE) {
            continue;
        }

        switch (line_type) {
            case MACRO_DEF:
                if (line_type == RESERVED_WORD_ERROR) {
                    print_error("Error: Extra characters in macro definition line", file->line);
                    *error_exist = 1;
                    continue;
                }
                if (strlen(macro_name) > MAX_MACRO_NAME_LENGTH) {
                    print_error("Error: Macro name is too long", file->line);
                    *error_exist = 1;
                    continue;
                }
                if (macro_open) {
                    print_error("Error: Nested macros not allowed", file->line);
                    *error_exist = 1;
                    continue;
                }
                macro_open = 1;
                macro_content[0] = '\0';

                while (fgets(line, sizeof(line), file_as) && strncmp(line, "endmacr", 7) != 0) {
                    file->line++;

                    // זיהוי והימנעות משורות הערה בתוך הגדרת המקרו
                    if (determine_line_type(line, list, &macro_name) == COMMENT_LINE) {
                        continue;
                    }

                    if (strlen(line) > MAX_LINE_LENGTH) {
                        print_error("Error: Line length exceeds 81 characters", file->line);
                        *error_exist = 1;
                        continue;
                    }
                    strcat(macro_content, line);
                }

                if (strncmp(line, "endmacr", 7) == 0 && strlen(line) > 7 && line[7] != '\n' && line[7] != '\0') {
                    print_error("Error: Extra characters in endmacro line", file->line);
                    *error_exist = 1;
                    macro_open = 0;
                    continue;
                }

                if (!macro_open) {
                    print_error("Error: endmacr without opening macro", file->line);
                    *error_exist = 1;
                    continue;
                }
                macro_open = 0;

                if (insert_new_macro(list, macro_name, macro_content, file) != 0) {
                    *error_exist = 1;
                }
                break;

            case MACRO_CALL:
                call_macro(macro_name, file_am, list, file);
                break;

            case MACRO_END_DEF:
                if (strncmp(line, "endmacr", 7) == 0 && strlen(line) > 7) {
                    print_error("Error: Extra characters in endmacro line", file->line);
                    *error_exist = 1;
                    continue;
                }
                if (!macro_open) {
                    print_error("Error: endmacr without opening macro", file->line);
                    *error_exist = 1;
                }
                macro_open = 0;
                break;

            default:
                if (line_type != COMMENT_LINE && strlen(line) <= MAX_LINE_LENGTH) {
                    fputs(line, file_am);  // כתיבת השורה לקובץ הפלט אם היא חוקית
                } else {
                    print_error("Error: Line length exceeds 81 characters", file->line);
                    *error_exist = 1;
                }
                break;
        }

        free(macro_name);
        macro_name = NULL;
    }

    if (macro_open) {
        print_error("Error: No endmacr for opened macro", file->line);
        *error_exist = 1;
    }
    // עיבוד שורות .data לאחר סיום עיבוד כל השורות האחרות
    process_data_lines(data_lines, num_data_lines, file_am);

    return 0;
}




// Function to create the output filename based on the input filename
void create_output_filename(char *input_filename, char *output_filename) {
    strcpy(output_filename, input_filename);
    char *dot = strrchr(output_filename, '.');
    if (dot != NULL) {
        strcpy(dot, ".am");
    } else {
        strcat(output_filename, ".am");
    }
}

// Function to check if a macro name already appeared
int macro_name_appeared(MacroList *list, const char *str) {
    MacroNode *current = list->head;
    while (current) {
        if (strcmp(current->name, str) == 0) {
            return TRUE;
        }
        current = current->next;
    }
    return FALSE;
}

//בודקת אם קיימת מילה במאגר סטרינגס
int is_reserved_word(const char *str) {
    int i;
    for (i = 0; i <RESERVED_WORD_NUM ; i++) {
        if (strcmp(strings[i], str) == 0)
            return TRUE; /* String exists in the array*/
    }
    return FALSE; /* String does not exist in the array*/
}


void process_data_lines(char *data_lines[], int num_data_lines, FILE *file_am) {
    for (int i = 0; i < num_data_lines; i++) {
        // כתיבה לקובץ הפלט
        fputs(data_lines[i], file_am);
        // שחרור הזיכרון שהוקצה לשורה זו
        free(data_lines[i]);
    }
}
