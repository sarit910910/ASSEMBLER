#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Errors.h"
#include "Reserved_words.h"
#include "Global_def.h"
#include "Symbols.h"
#include "NewMacro.h"
#include "First_pass.h"
#include "Arrays.h"
#include "Global_fun.h"

#define BIT_FIELD_SIZE 15//DEBUG

 /**
 * @brief Constructs a new filename by appending a given file ending to the original filename.
 * 
 * @param file_name Original filename.
 * @param ending The ending to append to the filename.
 * @return char* A pointer to the newly constructed filename. It is the responsibility of the caller to free this memory.
 *               Returns NULL if memory allocation fails.
 */   
char *name_of_file(char *file_name, char *ending) {
    char *new_file_name;
    new_file_name = (char*)malloc(MAX_NAME_LENGTH * sizeof(char));
    if (new_file_name == NULL)
    {
	print_internal_error(memory_failed);
        return NULL;
    }
    strcpy(new_file_name, file_name);
    /* adds the ending of the new file name */
    strcat(new_file_name, ending);
    return new_file_name;
}


  /**
 * @brief Main function to preprocess files.
 * 
 * @param argc Number of command-line arguments.
 * @param argv Array of command-line argument strings.
 * @return int Returns 0 upon successful execution, otherwise returns a non-zero value.
 */
int main(int argc, char * argv[])
{
    int error_exists = FALSE;
   struct macro_linked_list * list;
   struct file_status * file;
   int result;
   char * file_name = NULL;
   FILE * file_as;
   FILE * file_am;
   char * copy_file_as=NULL;
   char * path;
   if (argc < 2){
	print_internal_error(no_files);
    }

   while (--argc>0)
  {
    error_exists = FALSE;
    file_name=NULL;
     copy_file_as=NULL;
    list = create_macro_list();
    if (list == NULL){
	print_internal_error(memory_failed);
	return 1;
    }
/*if malloc failed*/
    
    if ((file_name = name_of_file(argv[argc],".as"))==NULL){
	free_macro_list(list);
        return 1;
    }
      printf("bbb %s\n", file_name);
    /*file_as = fopen(file_name,"r"); //DEBUG */
    file_as = fopen("C:\\Users\\sarit\\Downloads\\mmn 14\\source\\input","r");
    copy_file_as=file_name;
    if (file_as == NULL){
        printf("hhhhhh\n");
	free_macro_list(list);
	print_internal_error(no_such_file);
	free(file_name);
	continue;
     }

if ((file_name = name_of_file(argv[argc],".am"))==NULL){
	free_macro_list(list);
	fclose(file_as);
	free(copy_file_as);
        return 1;
    }
    file_am = fopen(file_name,"w+");





    file= (struct file_status *)malloc(sizeof(struct file_status));
    if (file == NULL){
	fclose(file_as);
	fclose(file_am);
	print_internal_error(memory_failed);
	free_macro_list(list);
	free(file_name);
	free(copy_file_as);
        return 1;
     }
	file->name=copy_file_as;
	file->line=0;
     	result = pre_proccesor_main(&error_exists,file,file_as,file_am,list);
     	fclose(file_as);
    	if ( result == EXTERNAL_ERROR){
     		fclose(file_am);
 		path= (char*)malloc((MAX_NAME_LENGTH+30)*sizeof(char));
		if (path==NULL){
            		fclose(file_as);
            		fclose(file_am);
           		free_macro_list(list);
            		free(file_name);
            		free(copy_file_as);
            		print_internal_error(memory_failed);
            		return 1;
        	}
		strcpy(path,"../project/");
		file->name[strlen(file->name) - 3] = '\0';	
		strcat(path,strcat(file->name,".am"));
		free_macro_list(list);
		free(file);
		free(copy_file_as);
      		if (remove(path)){ 
          		print_internal_error(remove_fail);   
      			}   
      		free(path);
      		free(file_name); 
      		continue;
    }
	
   if (result == INTERNAL_ERROR){
    free_macro_list(list);
    	free(file_name);
	 free(file);
	fclose(file_am);
	free(copy_file_as);
       return 1; 
   }
    fseek(file_am, 0, SEEK_SET);
    file->name=file_name;
    file->line=0;

    result= first_pass_main(list,&error_exists,file,file_am);
	free(copy_file_as);
        free(file_name);
     free_macro_list(list);
     free(file);
	fclose(file_am);
     if (result == INTERNAL_ERROR){
    	return 1;
     }
     
     /*going to the next file-next iteration*/
  }


	
   /*end of while-end of program*/
  return 0;
  
}



