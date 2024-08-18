#include "second_pass.h"
#include "Symbols.h"
#include "Arrays.h"
#include "First_pass.h"
#include "Global_def.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#define MAX_MEMORY 4096
#define MAX_OCTAL_LENGTH 6
static char memory[MAX_MEMORY][MAX_OCTAL_LENGTH]; /* Memory representation in octal */

/* Function to convert a binary string to an octal integer */
static int binary_to_octal(const char* binary) {
    int decimal = 0;
    int i;
    for (i = 0; i < 15; i++) {
        decimal = (decimal << 1) | (binary[i] - '0');  /* Convert binary to decimal */
    }
    return decimal;  /* Return the decimal value which will be converted to octal later */
}

/* Function to find a symbol in the symbols list */
struct symbols_node* find_symbol(struct symbols_linked_list *list, const char *label) {
    struct symbols_node *current = list->head;
    while (current != NULL) {
        if (strcmp(current->name, label) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

/* Function to process a single label from the label list */
static void process_label(LabelNode *labelNode, struct symbols_linked_list *symbols_list, bit_field *instructions_array) {
    struct symbols_node *current = symbols_list->head;
    int address = -1;
    int are = 0;

    // Find the label in the symbols list
    while (current != NULL) {
        if (strcmp(current->name, labelNode->label) == 0) {
            address = current->value;
            break;
        }
        current = current->next;
    }

    if (address == -1) {
        fprintf(stderr, "Error: Label '%s' not found in symbol table.\n", labelNode->label);
        return;
    }

    // Determine ARE (Absolute, Relocatable, External)
    if (current->update_attribute == EXTERNAL) {
        are = 1; // Binary: 001, External
        instructions_array[labelNode->address].data = are;  // Just the ARE value for external symbols
    } else {
        are = 2; // Binary: 010, Relocatable
      instructions_array[labelNode->address].data = (address << 3) | are;  // Address shifted left and ARE added
    }

    // Update entry file if it's an entry symbol
    if (current->is_entry==1) {
        FILE *ent_file = fopen("output.ent", "a");
        if (!ent_file) {
            fprintf(stderr, "Error: Could not open output.ent for writing.\n");
            return;
        }
        fprintf(ent_file, "%s %04d\n", current->name, address + 100);
        fclose(ent_file);
    }

    // Print debug information
    printf("Processed label: %s, Address: %d, Line: %d, ARE: %d, Is_entry: %d\n", labelNode->label, address, labelNode->address, are,current->is_entry);
}

/* Main function for the second pass */
void second_pass(date_structures *structures, DC_IC *locations) {
    LabelNode *currentLabel = structures->label_list;
    FILE *ext_file = fopen("output.ext", "w");
    FILE *ob_file = fopen("output.ob", "w");
    FILE *ent_file = fopen("output.ent", "w");  // פתיחה מראש של קובץ ה-ent

    if (!ext_file || !ob_file || !ent_file) {
        fprintf(stderr, "Error: Could not open output files.\n");
        return;
    }

    // Write the header to the .ob file
    fprintf(ob_file, "%d %d\n", locations->IC, locations->DC);

    // Process each label in the label list
    while (currentLabel != NULL) {
        process_label(currentLabel, structures->symbols_list, structures->instructions_array);

        // Update .ext file
        struct symbols_node *symbol = find_symbol(structures->symbols_list, currentLabel->label);
        if (symbol && symbol->update_attribute == EXTERNAL) {
            fprintf(ext_file, "%s %04d\n", symbol->name, currentLabel->address + 100);
        }

        // Move to next label
        currentLabel = currentLabel->next;
    }

    // Iterate over the symbol table for .entry symbols
    struct symbols_node *currentSymbol = structures->symbols_list->head;
    while (currentSymbol != NULL) {
        if (currentSymbol->is_entry == 1 && currentSymbol->update_attribute == RELOCATEABLE) {
            printf("ENT: %s %04d\n", currentSymbol->name, currentSymbol->value);
            fprintf(ent_file, "%s %04d\n", currentSymbol->name, currentSymbol->value );
        }
        currentSymbol = currentSymbol->next;
    }

    // Write the final instructions array to the .ob file
    for (int i = 0; i < locations->IC + locations->DC; i++) {
        fprintf(ob_file, "%04d %05o\n", i + 100, structures->instructions_array[i].data);
    }

    fclose(ext_file);
    fclose(ob_file);
    fclose(ent_file);

    printf("Second pass completed successfully.\n");
}