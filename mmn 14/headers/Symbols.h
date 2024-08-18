#ifndef SYMBOLS_H
#define SYMBOLS_H

#include <stdio.h>
#include <stdlib.h>
#include "First_pass.h"
#include "Global_def.h"


/* Struct for the node */
struct symbols_node {
	char *name;
	/*DC,IC .in the end of first_pass it will change*/
	int value;
	/*data,code*/
	int location;
	/*external,relocateable*/
	int update_attribute;
	int size;
	int line;
	int is_entry;
	struct symbols_node *next;
};

/* Struct for the linked list */
struct symbols_linked_list {
	struct symbols_node *head;
	struct symbols_node *tail;
};






/**
 * @brief Creates a new linked list.
 *
 * @return Pointer to the newly created linked list.
 */
struct symbols_linked_list* create_symbols_linked_list();


/**
 * Insert a new symbol node into the symbols linked list.
 *
 * This function inserts a new symbol node into the symbols linked list.
 *
 * @param list A pointer to the symbols linked list where the new node will be inserted.
 * @param new_symbols_node A pointer to the new symbol node to be inserted.
 */
void insert_new_symbol(struct symbols_linked_list *list, struct symbols_node * new_symbols_node);

/**
 * Check if a symbol appears in the symbols linked list and perform related actions.
 *
 * This function checks if a symbol appears in the symbols linked list and performs
 * actions based on the provided parameters.
 *
 * @param list A pointer to the symbols linked list to be checked.
 * @param symbol_name A pointer to the symbol name to be checked.
 * @param type An integer representing the type of symbol.
 * @param file A pointer to the file status structure.
 * @param errors A pointer to the errors status structure.
 *
 * @return Returns an integer indicating the result of the check and related actions:
 *         - TRUE if there is error in this label or define decleration.
 *         - FALSE if this name is fine considering various conditions.
 */
int appear_in_symbols(struct symbols_linked_list *list, char * symbol_name,int type,struct file_status * file , errors_status *errors);
/**
 * Set an entry point for a symbol in the linked list of symbols.
 * 
 * @param list Pointer to the linked list of symbols.
 * @param name Name of the symbol.
 * @param file Pointer to the file status structure.
 * @return Returns TRUE if the entry point is set successfully, FALSE otherwise.
 */
int set_entry(struct symbols_linked_list * list,char * name,struct file_status * file);


/**
 * @brief Adds a new node to the end of the linked list.
 *
 * @param list Pointer to the linked list.
 * @param name Pointer to the symbol name string.
 * @param value The value of this symbol
 * @param location code ,data 
 * @param update_attribute relocateable or external
 * @param size The size in the memory that this symbol takes
 * @param line The line that this symbols declared in
 * 
 * @return void
 */
 int create_insert_symbol(struct symbols_linked_list * list,char *name, int value,int location,int update_attribute,long size,int line, int is_entry);

/**
 * @brief Frees the memory allocated for the linked list and its nodes.
 *
 * @param list Pointer to the linked list to be freed.
 * 
 * @return void
 */
void free_symbols_list(struct symbols_linked_list *list);

/**
 * Set an entry point for a symbol in the linked list of symbols.
 * 
 * @param list Pointer to the linked list of symbols.
 * @param name Name of the symbol.
 * @param file Pointer to the file status structure.
 * @return Returns TRUE if the entry point is set successfully, FALSE otherwise.
 */
int set_entry(struct symbols_linked_list * list,char * name,struct file_status * file);

void print_list(struct symbols_linked_list *list);

void remove_spaces(char *str);

int compare_strings_n(const char *str1, const char *str2, size_t n);

#endif /* SYMBOLS_H */
