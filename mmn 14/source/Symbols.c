#include "Errors.h"
#include "Symbols.h"
#include "Global_def.h"
#include "Global_fun.h"
#include "First_pass.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

//
 /*Function to print the contents of the linked list */
void print_list(struct symbols_linked_list *list) {//DEBUG
    struct symbols_node *current = list->head;
    while (current != NULL) {
        printf("Name: %s, Value: %d, Location: %d, Update Attribute: %d, Size: %d, Line: %d, Is Entry: %d\n",
               current->name, current->value, current->location, current->update_attribute,
               current->size, current->line, current->is_entry);
        current = current->next;
    }
}

//


int appear_in_symbols(struct symbols_linked_list *list, char *symbol_name, int type, struct file_status *file,errors_status *errors) {
    struct symbols_node *current = list->head;
    while (current != NULL) {
        if (strcmp(current->name, symbol_name) == 0) {
            switch (type) {
                case REGULAR:
                    /**errors = double_label;        שיניתי מזה בתאריך 4.8   */
                    errors->external_error_exist = double_label;
                    print_external_error(double_label, file);
                    return TRUE; /* Same name */
                case EXTERN:
                    if (current->update_attribute != EXTERNAL) {
                        /**errors = double_label;        שיניתי מזה בתאריך 4.8   */
                    errors->external_error_exist = double_label;
                        print_external_error(double_label, file);
                        return TRUE; /* Appears but not as extern, so it's a problem */
                    }
                    /* If we are here, it means that we had this extern already so we don't want to enter it again */
                    print_warning(w_extern_double, file);
                    return TRUE;
                    break;
            }
        }
        current = current->next;
    }
    return FALSE; /* There is no problem */
}

void insert_new_symbol(struct symbols_linked_list *list, struct symbols_node * new_symbols_node){
    new_symbols_node->next=NULL;
   // new_symbols_node->is_entry=FALSE;
    if (list->head == NULL) {
        list->head = new_symbols_node;
        list->tail = new_symbols_node;
    } else {
        list->tail->next = new_symbols_node;
        list->tail = new_symbols_node;
    }
    
}


/* Function to initialize the linked list  הוספתי 4.8*/
struct symbols_linked_list* create_symbols_linked_list(){
    /*dynamically allocates memory for a new linked list*/
    struct symbols_linked_list * newList = (struct symbols_linked_list *)malloc(sizeof(struct symbols_linked_list));
    if (newList == NULL) {
        print_internal_error(memory_failed);
        return NULL;
    }
    /*initializes its head and tail pointers to NULL*/
    newList->head = NULL;
    newList->tail = NULL;
    return newList;
}


/* Function to free memory allocated for the linked list הוספתי 4.8*/
void free_symbols_list(struct symbols_linked_list *list) {
 /* This function iterates through each node in the linked list,
 * frees the memory allocated for the symbol name,
 * and finally frees the memory allocated for each node and the linked list itself.*/
    struct symbols_node *current = list->head;
    while (current != NULL) {
        struct symbols_node *temp = current;
        current = current->next;
        free(temp->name);
        free(temp);
    }
    free(list);
}

 int create_insert_symbol(struct symbols_linked_list * list,char *name, int value,int location,int update_attribute,long size,int line, int is_entry){
    /*dynamically allocates memory for a new node*/
    struct symbols_node * new_symbols_node = (struct symbols_node*)malloc(sizeof(struct symbols_node));
    if (new_symbols_node == NULL) {
       
      print_internal_error(memory_failed);
        return INTERNAL_ERROR; 
    }
    /*copies the symbol name and content strings*/
    new_symbols_node->name = my_strdup(name);
    if (name == NULL){
	free(new_symbols_node);
	return INTERNAL_ERROR; 
    }
    new_symbols_node->value = value;
    new_symbols_node->location = location;
    new_symbols_node->update_attribute=update_attribute;
    new_symbols_node->size=size;
    new_symbols_node->line=line;
    new_symbols_node->is_entry=is_entry;
    new_symbols_node->next=NULL;
    insert_new_symbol(list,new_symbols_node);
	return NO_ERROR;
}


int set_entry(struct symbols_linked_list * list,char * name,struct file_status * file){
    struct symbols_node * current = list->head;
    /* Traverse the linked list*/
    while (current != NULL) {
        /* Check if the name matches*/
        remove_spaces(name);
        remove_spaces(current->name);
        int len1 = strlen(name);
        int len2 = strlen(current->name);
        if (compare_strings_n(current->name, name,len2)) {
             if (current->update_attribute == EXTERNAL){
                print_external_error(double_external_entry,file);
                return FALSE;
            }
            else{
                current->is_entry= TRUE;
		return TRUE;
            }

        }
        current = current->next; /* Move to the next node*/
    }
    print_external_error(entry_un_realized,file);
    return FALSE; /* this entry never realized*/
}

void remove_spaces(char *str) {
    int i, j = 0;
    int len = strlen(str);

    for (i = 0; i < len; i++) {
        if (str[i] != ' ') { // בודקים אם התו הנוכחי אינו רווח
            str[j++] = str[i]; // מעתיקים את התו למיקום חדש במחרוזת
        }
    }
    }


int appear_in_symbols_withNmae(struct symbols_linked_list *list, char *symbol_name,  struct file_status *file) {
    struct symbols_node *current = list->head;
    int x=1;
    while (current != NULL)
    {
        if (strcmp(current->name, symbol_name) == 0)
        return x;
        x++;
         current = current->next;
    }
    return -1;
}


int compare_strings_n(const char *str1, const char *str2, size_t n) {
    // השוואה של n תווים בין str1 ל-str2
    for (size_t i = 0; i < n; i++) {
        if (str1[i] != str2[i]) {
            return 0; // המחרוזות שונות
        }
        // אם הגענו לסוף של אחת המחרוזות לפני n תווים, צריך לבדוק את זה
        if (str1[i] == '\0' || str2[i] == '\0') {
            break;
        }
    }
    return 1; // המחרוזות שוות עד n תווים
}