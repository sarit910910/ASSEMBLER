#include "Global_def.h"
#include "Global_fun.h"
#include "Symbols.h"
#include "Opcodes_table.h"
#include "second_pass.h"
#include "Arrays.h"
#include "Errors.h"
#include "First_pass.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>

int first_pass_main(struct macro_linked_list * list, int * error_exist,struct file_status * file,FILE * file_am){
    file->line=0; 
    int found_EOF=FALSE ,i,size/*גודל המערך של התוויות*/;
    char ** ptp;
    char * buffer,*copy;
    DC_IC * locations;
    date_structures * structures;
    errors_status * errors;
    LabelNode *labelList ;
    labelList= (LabelNode *)malloc(sizeof(LabelNode));
    if (labelList==NULL){
        print_internal_error(memory_failed);
        return INTERNAL_ERROR;
    }
    /*coz send to function returns struct for symbols the symbols*/
    struct symbols_node* fictive = NULL;
    locations= (DC_IC *)malloc(sizeof(DC_IC));
    if (locations==NULL){
        print_internal_error(memory_failed);
        return INTERNAL_ERROR;
    }

    locations->DC=0;
    locations->IC=0;
    structures= (date_structures *)malloc(sizeof(date_structures));
    if (structures==NULL){
        print_internal_error(memory_failed);
        free(locations);
        return INTERNAL_ERROR;
    }

    structures->macro_list= list;
    structures->symbols_list= create_symbols_linked_list();
    if (structures->symbols_list==NULL){
        print_internal_error(memory_failed);
        free(locations);
	free(structures);
        return INTERNAL_ERROR;
    }

    /*we need to initialize the arrays*/
    structures->data_array=NULL;
    structures->instructions_array=NULL;
    errors= (errors_status *)malloc(sizeof( errors_status));
    if (errors==NULL){
        print_internal_error(memory_failed);
        free(locations);
	free_symbols_list(structures->symbols_list);
	free(structures);
        return INTERNAL_ERROR;
    }

    structures->data_array=NULL;
    structures->instructions_array=NULL;
    errors->external_error_exist=*error_exist;
    errors->internal_error_exist=FALSE;
    buffer = (char *)malloc((MAX_LINE_LENGTH) * sizeof(char));
     if (buffer == NULL){
            free(locations);
	    free_symbols_list(structures->symbols_list);
	    free(structures);
	    free(errors);
            print_internal_error(memory_failed);
            return INTERNAL_ERROR;
      }
   
   while (!found_EOF)
   {
        file->line++;
	buffer = (char *)malloc((MAX_LINE_LENGTH) * sizeof(char));
     	if (buffer == NULL){
            free(locations);
	    free_symbols_list(structures->symbols_list);
	    free(structures);
	    free(errors);
            print_internal_error(memory_failed);
            return INTERNAL_ERROR;
         }
        found_EOF=fill_and_check(buffer,file_am);
        if (feof(file_am)){
            found_EOF = TRUE;
        }
        copy = buffer;
	ptp = &copy;
        fictive = send_to_function(FALSE,ptp,file,errors,locations,structures);
        free(fictive);
        if (errors->internal_error_exist == TRUE){
            free(locations);
	    free_symbols_list(structures->symbols_list);
	    free(structures->data_array);
        freeLabelList(structures->label_list);
    	free(structures->instructions_array);
	    free(structures);
	    free(errors);
            return INTERNAL_ERROR;
        }	
        free(buffer);
    }
    

    end_of_pass_1(locations,structures,file,file_am); 
        print_list(structures->symbols_list);//DEBUG

    for(i=0;i<locations->IC;i++)//DEBUG
    {//DEBUG
        printBinary15Bit(structures->instructions_array[i].data);//DEBUG
    }//DEBUG
    //entries(file,file_am,structures,locations);
    printf("---------------------------");
print_list(structures->symbols_list);//DEBUG
    printLabelList(structures->label_list); 
    second_pass(structures, locations);
    free(buffer);
    free(locations);
    free_symbols_list(structures->symbols_list);
    free(structures->data_array);
    free(structures->instructions_array);
    free(structures);
    if (errors->internal_error_exist == TRUE){
        return INTERNAL_ERROR;
    }
    free(errors);
    return NO_ERROR;
}


struct symbols_node* send_to_function(int is_label,char ** ptp,struct file_status * file, errors_status * errors,DC_IC * locations,date_structures * structures ){
    struct symbols_node* node = NULL;
    int index_of_action;
	int word_length;
    char * first_word = next_word(ptp);
    /*notice-  ptp moved*/
    if (first_word== NULL){
        errors->internal_error_exist=TRUE;
        return NULL;
    }
    word_length=strlen(first_word);
    /*coz maybe first word is just the EOF*/
    if (word_length < 1){
	  return NULL;
    }
     /*coz if its label we got label into label*/
    if (!strcmp(first_word,".entry")){
        free(first_word);
        /*we want skip entries*/
        return NULL;
    }
    else if(!is_label && first_word[ word_length -1 ]==':'){
        /*send the name of label*/
        /*remove the last :,by override it */
        first_word[word_length-1] ='\0';
        node = label(first_word,ptp,file,errors,locations,structures);
    }
    else if (!strcmp(first_word,".extern")){
        node = extern_line(ptp,file,errors,locations,structures);
    }
    else if (!strcmp(first_word,".data")){
        node = data(ptp,file,errors,locations,structures);
    }
    else if (!strcmp(first_word,".string")){
        node = string(ptp,file,errors,locations,structures);
    }
    else if ((index_of_action=index_action(first_word)) != FICTIVE){
        /* if its not fictive its action ,we send the index action*/
        node = action(index_of_action,ptp,file,errors,locations,structures);
    }
    else{
        /*to print that there is problem with the line. it can be lot of thing so print something general*/
        print_external_error(unrocognize_line,file);
        errors->external_error_exist= TRUE;
    }
    free(first_word);
    return node;
}


struct symbols_node* label(char * label,char ** ptp ,struct file_status * file, errors_status * errors,DC_IC * locations,date_structures * structures ){
    struct symbols_node * node;
    int result;
    char * first_word = next_word(ptp);
    char * st = ".entry";
    result=strcmp(first_word,st);
    if(result==0)/*זה אומר שהמילה הראשונה היא איסאנטרי וצרי לעדכן את זה*/
    {
        node->is_entry=TRUE;
    }
    /*this values maybe will change*/
    int type=REGULAR;
    if (!legal_word(label,FALSE,structures->macro_list,file)){
        errors->external_error_exist=TRUE;
        return NULL;
    }
    /*we need to skip spaces coz send_to_fun get only without spaces. if skip return false it means we have nothing after*/
    if(skip_spaces(ptp) == FALSE){
        print_external_error(empty_label,file);
        errors->external_error_exist=TRUE;
        return NULL;
    }
    node = send_to_function(TRUE,ptp,file,errors,locations,structures);
    if (node==NULL){
        /*it means that it was define or entry or extern or there was error*/
        return NULL;
    }
    
    if (node->update_attribute==EXTERNAL){
        type = EXTERN;
    }
    if (appear_in_symbols(structures->symbols_list,label,type,file,errors)){
	free(node);
        return NULL;
    }
    node->name= my_strdup(label);
    if (node->name == NULL){
	free(node);
        errors->internal_error_exist=TRUE;
        return NULL;
    }
    insert_new_symbol(structures->symbols_list,node);
	return NULL;
}

struct symbols_node* extern_line(char ** ptp ,struct file_status * file, errors_status * errors,DC_IC * locations,date_structures * structures ){
    char * label;
    /*skip coz send to fun get only without spaces. is skip return false it means we have nothing after*/
    if(skip_spaces(ptp) == FALSE){
        print_external_error(empty_extern,file);
        errors->external_error_exist=TRUE;
        return NULL;
    } 
    label = next_word(ptp);
    if (label == NULL){
        errors->internal_error_exist=TRUE;
        return NULL;
    }
    /*0 so that will check from begining*/
    if (!valid_end(*ptp,0,file, &(errors->external_error_exist)) || !legal_word(label,FALSE,structures->macro_list,file) ){
	free(label);
        errors->external_error_exist=TRUE;
        return NULL;
    }
	if (appear_in_symbols(structures->symbols_list,label,EXTERN,file,errors)){
		free(label);
		return NULL;
        }
    /*the value is 0 coz its extern. the location and size are fictive. we will know not to add 100 coz its extern*/
    if (create_insert_symbol(structures->symbols_list,label,0,FICTIVE,EXTERNAL,FICTIVE,file->line,FALSE) == INTERNAL_ERROR){
	free(label);
        errors->internal_error_exist=TRUE;
    }
    return NULL;/*coz extern never in a label*/
}

struct symbols_node* data(char ** ptp ,struct file_status * file, errors_status * errors,DC_IC * locations,date_structures * structures ){
    struct symbols_node* node=(struct symbols_node*)malloc(sizeof(struct symbols_node));
    char * param,* param2,*check_param; /* Pointer to store the position of the first invalid character*/
    long num=0;
    int end = FALSE;
    if (node==NULL){
	print_internal_error(memory_failed);
        errors->internal_error_exist=TRUE;
        return NULL;
    }
    node->value=locations->DC;
    node->location= DATA;
    node->update_attribute=RELOCATEABLE;
    node->line=file->line;
    while(!end){
        if(skip_spaces(ptp) == FALSE){
            /*in the first time it means we have no data at all and after that it means we had comma but dont param after this*/
            print_external_error(missing_parameter,file);
            errors->external_error_exist=TRUE;
            free(node);
            return NULL;
        }
        /*skip spaces not false so sure we hava something till the end*/
        param = next_param(ptp);
        if (param==NULL){
	   free(node);
	   print_internal_error(memory_failed);
            errors->internal_error_exist=TRUE;
            return NULL;
        }
        param2 = param;
        if(param2[0] == '#'){
            /*skip this char*/
            param2++;
        }
        /*The third argument is the base of the number system to convert from (typically 10 for decimal).*/
         num = strtol(param2, &check_param, 10); /*Convert string to long*/ 

        if (param2 == check_param || *check_param != '\0'){
            //לשנות פה את הודעת השגיאה שלא תהיה עם דיפיין... חובה!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!DEBUG
            print_external_error(no_such_define,file);
                errors->external_error_exist=TRUE;
                free(node);
		free(param);
                return NULL;
        }

		if (num > MAXIMUM_VALUE || num < MINIMUM_VALUE){
        		print_external_error(too_big_number,file);
        		errors->external_error_exist=TRUE;
			free(node);
	   		 free(param);
           		 return NULL;
    		}
            /*we got number...*/
            insert_data(num,file,errors,locations,structures);
        /*checking that the insert went well*/
        if ( errors->internal_error_exist== TRUE){
            /* error happend*/
            free(node);
	    free(param);
            return NULL;
        }
        if (skip_spaces(ptp)==FALSE){
            /*we reach end of input or*/
            end = TRUE;
        }
        else{
            /*we didnt reach end of input*/
            if (**ptp == ','){
                /*skip the comma to check the parameter that come after this*/
                (*ptp)++;
            }
            else{
                /*we had parameter and then space and now something that its not comma so we are missing comma*/
                print_external_error(missing_comma,file);
                errors->external_error_exist=TRUE;
                free(node);
		free(param);
                return NULL;
            }
        }
    }
    /*it represent how much DC grew here*/
    node->size= (locations->DC)-(node->value);
    free(param);
    return node;  
}

void insert_data(int value,  struct file_status * file, errors_status * errors,DC_IC * locations,date_structures * structures ){
    /*for size I want DC + 1 for the 0 in the end of string*/
    int *dataBinary;
    int index_code, index_arr;
    
    structures->data_array= (bit_field *)realloc(structures->data_array,(locations->DC +1)*sizeof(bit_field));
    if (structures->data_array == NULL){
        print_internal_error(memory_failed);
        errors->internal_error_exist=TRUE;
    }
	else{
    /*for index I want DC- coz indexes begin in 0*/
    (structures->data_array)[locations->DC].data= value;
    locations->DC=locations->DC+1;
	}
    dataBinary=decimalToBinary(value,15);
     index_code=14,index_arr=0;
        for(;index_arr <15;index_arr++){
            	structures->instructions_array= (bit_field *)realloc(structures->instructions_array,(locations->IC + 1)*sizeof(bit_field));//DEBUG
	      	set_bit(&(structures->instructions_array)[locations->IC],index_code,dataBinary[index_arr]);
	    	index_code--;
	    }
        locations->IC++;/*הגדלנו את המונה ב-1 כדי להכניס קידוד של הערך שהתקבל בדאטה*/
}


struct symbols_node* string(char ** ptp ,struct file_status * file, errors_status * errors,DC_IC * locations,date_structures * structures ){
    struct symbols_node* node=(struct symbols_node*)malloc(sizeof(struct symbols_node));
    if (node==NULL){
        print_internal_error(memory_failed);
        errors->internal_error_exist=TRUE;
        return NULL;
    }
    node->value=locations->DC;
    node->location= DATA;
    node->update_attribute=RELOCATEABLE;
    node->line=file->line;

    if(skip_spaces(ptp) == FALSE){
        /* it means we have no string at all*/
        print_external_error(missing_parameter,file);
        errors->external_error_exist=TRUE;
        free(node);
        return NULL;
    }
    if ((**ptp) != '"'){
        /*there is no "  at first*/
        print_external_error(string_with_invalid_begin,file);
        errors->external_error_exist=TRUE;
        free(node);
        return NULL;
    }
    /*skip the "*/
    (*ptp)++;
    while ( **ptp != '\n' && ( **ptp != '"' || !isspace((*ptp)[1]) ) ){
        if(!isprint(**ptp)){
            print_external_error(invalid_char_in_string,file);
            errors->external_error_exist=TRUE;
            free(node);
            return NULL;
        }
         printf("IC DATA: BEFORE %d\n", locations->DC);//DEBUG
        insert_data((int)(**ptp), file,errors,locations,structures);
         printf("IC STRING: AFTER %d\n", locations->DC);//DEBUG
        (*ptp)++;
       
	}
    
    /*we are out of the loop while because we reach \n or " with space after it*/
    if (**ptp != '"'){
        /*we out of the while not because we reaches " ,but because we reached end of line */
        print_external_error(missing_ending_char,file);
        errors->external_error_exist=TRUE;
        free(node);
        return NULL;
    }
    /*skip the "*/
    (*ptp)++;
    if (!valid_end(*ptp,0,file,&(errors->external_error_exist))){
        /*we out of the while because we reaches " ,but there is extra text after that*/
        print_external_error(missing_ending_char,file);
        free(node);
        return NULL;
    }
    node->size= (locations->DC)-(node->value);
    return node;
}
struct symbols_node* action(int index_of_action,char ** ptp ,struct file_status * file, errors_status * errors,DC_IC * locations,date_structures * structures ){
    /*this is num of words the this line will take in the instructuon array*/
    int L ,i, destination_operand_method, source_operand_method,index_data,index_binary_num,number,number2;
    char * destination_operand, * source_operand;
    int *binaryNum,*binaryNum2;
    char * operand,*opernd2;/*בשביל הקידוד בהמשך של היעד*/
    struct symbols_node* node=(struct symbols_node*)malloc(sizeof(struct symbols_node));
    if (node==NULL){
	print_internal_error(memory_failed);
        errors->internal_error_exist=TRUE;
        return NULL;
    }
    node->value=locations->IC;
    node->location= CODE;
    node->update_attribute=RELOCATEABLE;
    node->line=file->line;
    if(skip_spaces(ptp) == FALSE){
        /*so we dont hava params*/
        destination_operand=NULL;
        source_operand=NULL;
    }
    else{
        /*so we have parameters ahead*/
        source_operand= next_param(ptp);
	if (source_operand==NULL){
		print_internal_error(memory_failed);
             errors->internal_error_exist=TRUE;
		free(node);
             return NULL;
	}
        if(skip_spaces(ptp) == FALSE){
            /*so we dont hava 2 param*/
            /* if we hava just 1 opernad it consider destination_operand*/
            destination_operand=source_operand;
            source_operand=NULL;
        }
        else{
            /*so we have more parameter*/
		if (**ptp != ','){
			print_external_error(missing_comma,file);
        		errors->external_error_exist=TRUE;
			free(node);
			free(source_operand);
			return NULL;
		}
		/*so we got comma between*/
		(*ptp)++;	
		if(skip_spaces(ptp) == FALSE){
            		/*so we dont hava param after the comma*/
            		print_external_error(missing_param_after_comma,file);
        		errors->external_error_exist=TRUE;
			free(node);
			free(source_operand);
			return NULL;
        	}	
            	destination_operand= next_param(ptp);
	     	if (destination_operand==NULL){
			print_internal_error(memory_failed);
             		errors->internal_error_exist=TRUE;
			free(node);
			free(source_operand);
             		return NULL;
	        }
        }
        if (!valid_end(*ptp,0,file, &(errors->external_error_exist))){
            free(node);
	        free_strings(2,destination_operand,source_operand);
            return NULL;
        }
    }
    destination_operand_method= figure_addressing_methods(destination_operand,structures->symbols_list);
    source_operand_method=figure_addressing_methods(source_operand,structures->symbols_list);
    char * des =destination_operand;
    char * sur = source_operand;
    L= calculate_L_and_check(index_of_action,destination_operand_method,source_operand_method);
    if (L==FICTIVE){
        /*this action gots wrong parameters here*/
        print_external_error(wrong_param_fun,file);
        errors->external_error_exist=TRUE;
        free(node);
	free_strings(2,destination_operand,source_operand);
        return NULL;
    }
    /* save place in the array according to L*/
	structures->instructions_array= (bit_field *)realloc(structures->instructions_array,(locations->IC + L)*sizeof(bit_field));
    if (structures->instructions_array == NULL){
        print_internal_error(memory_failed);
        errors->internal_error_exist=TRUE;
	free(node);
	free_strings(2,destination_operand,source_operand);
        return NULL;
    }
    /*initialize to zero*/
    for (i=0;i<L;i++){
        (structures->instructions_array)[locations->IC + i].data= 0;
    }
    /*code the first word in the array*/

    code_opcode(index_of_action,locations,structures);
    code_operand_method(DESTINATION,destination_operand_method,locations,structures,structures->symbols_list,des);
    code_operand_method(SOURCE,source_operand_method,locations,structures,structures->symbols_list,sur);

    

    /*    מילת הוראה ללא אופרנדים כמו סטום כבר קודדה...*/
    if(L==2 && source_operand==NULL)/*זה אומר שיש רק אופרנד אחד ונקודד אותו לפי יעד*//*טיפול בכל סועגי המיעונים חות מסוג מספר 1*/
    {
        if(destination_operand_method==0)/*מיעון מיידי*/
          {
          operand=destination_operand;
            operand++;/*דילוג על סולמית*/
           number=atoi(operand);
           binaryNum= decimalToBinary(number,14-START_BIT_DESTINATION_REGISTER+1);/*המערך מקבל מערך בגודל מספר הסיביות הדרושות עם האופרנד  */
            index_data=14;
            index_binary_num=0;
            for(;index_binary_num <14-START_BIT_DESTINATION_REGISTER+1;index_binary_num++){
	      	set_bit(&(structures->instructions_array)[locations->IC+1],index_data,binaryNum[index_binary_num]);
                                    printf("Iteration imm %d: instruction value: %d, set bit %d to %d\n",index_binary_num,(structures->instructions_array)[locations->IC+1].data,index_data,binaryNum[index_binary_num]);
	    	index_data--;
	        }
            set_bit(&(structures->instructions_array)[locations->IC+1], 0, 0);/*ARE*/
            set_bit(&(structures->instructions_array)[locations->IC+1], 1, 0);
            set_bit(&(structures->instructions_array)[locations->IC+1], 2, 1);
            }
          else if(destination_operand_method==2 || destination_operand_method==3)/*מיעון אוגר עקיף או ישיר*/
          {
            operand=destination_operand;
            if(operand[0]=='*')
            operand++;/*דילוג אם יש כוכבית*/
            operand++;/*דילוג על האות אר גם במיעון 2 וגם ב3, כדי שיישאר לנו מספר האוגר*/
             number=atoi(operand);
            binaryNum= decimalToBinary(number,14-START_BIT_DESTINATION_REGISTER+1);/*המערך מקבל מערך בגודל מספר הסיביות הדרושות עם האופרנד  */
             index_data=14,index_binary_num=0;
            for(;index_binary_num <14-START_BIT_DESTINATION_REGISTER+1;index_binary_num++){
	      	set_bit(&(structures->instructions_array)[locations->IC+1],index_data,binaryNum[index_binary_num]);
            printf("Iteration %d: instruction value: %d, set bit %d to %d\n",index_binary_num,(structures->instructions_array)[locations->IC+1].data,index_data,binaryNum[index_binary_num]);
	    	index_data--;
	        }
            set_bit(&(structures->instructions_array)[locations->IC+1], 0, 0);/*ARE*/
            set_bit(&(structures->instructions_array)[locations->IC+1], 1, 0);
            set_bit(&(structures->instructions_array)[locations->IC+1], 2, 1); 
            
            }
            else if(destination_operand_method==1)/*תווית מיעון ישיר*/
            {
                operand=destination_operand;
                addLabelNode(&structures->label_list, operand, locations->IC+1);
            }
            
    }
          
    
    else if(L==2 && destination_operand_method !=4 && source_operand_method!=4)/*זה אומר שיש 2 אופרנדים מסוג עקיף או ישיר ונקודד אותם באותה מילה*/
    {
         operand=destination_operand;
            if(operand[0]=='*')
                operand++;/*דילוג אם יש כוכבית*/
            operand++;/*דילוג על האות אר גם במיעון 2 וגם ב3, כדי שיישאר לנו מספר האוגר*/
             number=atoi(operand);
         opernd2=source_operand;
            if(operand[0]=='*')
                opernd2++;/*דילוג אם יש כוכבית*/
            opernd2++;/*דילוג על האות אר גם במיעון 2 וגם ב3, כדי שיישאר לנו מספר האוגר*/
             number2=atoi(opernd2);
            binaryNum= decimalToBinary(number,3);/*המערך מקבל מערך בגודל מספר הסיביות הדרושות עם האופרנד  */
            binaryNum2=decimalToBinary(number2,3);/*המערך מקבל מערך בגודל מספר הסיביות הדרושות עם האופרנד  */


            /*קידוד היעד*/
             index_data=5,index_binary_num=0;
            for(;index_binary_num <3;index_binary_num++){
	      	set_bit(&(structures->instructions_array)[locations->IC+1],index_data,binaryNum[index_binary_num]);
	    	index_data--;
	        }
            set_bit(&(structures->instructions_array)[locations->IC+1], 0, 0);/*ARE*/
            set_bit(&(structures->instructions_array)[locations->IC+1], 1, 0);
            set_bit(&(structures->instructions_array)[locations->IC+1], 2, 1); 
            
            /*קידוד המקור*/
             index_data=8,index_binary_num=0;
            for(;index_binary_num <3;index_binary_num++){
	      	set_bit(&(structures->instructions_array)[locations->IC+1],index_data,binaryNum2[index_binary_num]);
	    	index_data--;
	        }
            set_bit(&(structures->instructions_array)[locations->IC+1], 0, 0);/*ARE*/
            set_bit(&(structures->instructions_array)[locations->IC+1], 1, 0);
            set_bit(&(structures->instructions_array)[locations->IC+1], 2, 1); 
     }
        
    
    else{ /*L==3 זאת אומרת שיש 2 אופרנדים וצריך לקודד עוד 2 שורות*/
        if(source_operand_method==0)/*אם זה מיידי*/
        {
            operand=source_operand;
            operand++;/*דילוג על סולמית*/
             number=atoi(operand);
            binaryNum= decimalToBinary(number,14-START_BIT_SOURCE_REGISTER+1);/*המערך מקבל מערך בגודל מספר הסיביות הדרושות עם האופרנד  */
             index_data=14,index_binary_num=0;
            for(;index_binary_num <14-START_BIT_SOURCE_REGISTER+1;index_binary_num++){
	      	set_bit(&(structures->instructions_array)[locations->IC+1],index_data,binaryNum[index_binary_num]);
	    	index_data--;
	        }
            set_bit(&(structures->instructions_array)[locations->IC+1], 0, 0);/*ARE*/
            set_bit(&(structures->instructions_array)[locations->IC+1], 1, 0);
            set_bit(&(structures->instructions_array)[locations->IC+1], 2, 1);
        }
        else if(source_operand_method==2 || source_operand_method==3)
        {
            operand=source_operand;
            if(operand[0]=='*')
            operand++;/*דילוג אם יש כוכבית*/
            operand++;/*דילוג על האות אר גם במיעון 2 וגם ב3, כדי שיישאר לנו מספר האוגר*/
            number = atoi(operand);
            binaryNum= decimalToBinary(number,14-START_BIT_SOURCE_REGISTER+1);/*המערך מקבל מערך בגודל מספר הסיביות הדרושות עם האופרנד  */
             index_data=14,index_binary_num=0;
            for(;index_binary_num <14-START_BIT_SOURCE_REGISTER+1;index_binary_num++){
	      	set_bit(&(structures->instructions_array)[locations->IC+1],index_data,binaryNum[index_binary_num]);
	    	index_data--;
	        }
            set_bit(&(structures->instructions_array)[locations->IC+1], 0, 0);/*ARE*/
            set_bit(&(structures->instructions_array)[locations->IC+1], 1, 0);
            set_bit(&(structures->instructions_array)[locations->IC+1], 2, 1); 
        }
        else if(source_operand_method==1)
        {
                operand=source_operand;
                addLabelNode(&structures->label_list, operand, locations->IC+1);
        }
        if(destination_operand_method==0)/*מיעון מיידי*/
        {
            operand=destination_operand;
            operand++;/*דילוג על סולמית*/
             number=atoi(operand);
            binaryNum= decimalToBinary(number,14-START_BIT_DESTINATION_REGISTER+1);/*המערך מקבל מערך בגודל מספר הסיביות הדרושות עם האופרנד  */
             index_data=14,index_binary_num=0;
            for(;index_binary_num <14-START_BIT_DESTINATION_REGISTER+1;index_binary_num++){
	      	set_bit(&(structures->instructions_array)[locations->IC+2],index_data,binaryNum[index_binary_num]);
	    	index_data--;
	        }
            printf("ic2 %d\n",locations->IC+2);//DEBUG
            set_bit(&(structures->instructions_array)[locations->IC+2], 0, 0);/*ARE*/
            set_bit(&(structures->instructions_array)[locations->IC+2], 1, 0);
            set_bit(&(structures->instructions_array)[locations->IC+2], 2, 1);
        }
        else if(destination_operand_method==2 ||destination_operand_method==3 )/* מיעון אוגר עקיף או ישיר*/
        {
            operand=destination_operand;
            if(operand[0]=='*')
            operand++;/*דילוג אם יש כוכבית*/
            operand++;/*דילוג על האות אר גם במיעון 2 וגם ב3, כדי שיישאר לנו מספר האוגר*/
             number=atoi(operand);
            binaryNum= decimalToBinary(number,14-START_BIT_DESTINATION_REGISTER+1);/*המערך מקבל מערך בגודל מספר הסיביות הדרושות עם האופרנד  */
             index_data=14,index_binary_num=0;
            for(;index_binary_num <14-START_BIT_DESTINATION_REGISTER+1;index_binary_num++){
	      	set_bit(&(structures->instructions_array)[locations->IC+2],index_data,binaryNum[index_binary_num]);
	    	index_data--;
	        }
            set_bit(&(structures->instructions_array)[locations->IC+2], 0, 0);/*ARE*/
            set_bit(&(structures->instructions_array)[locations->IC+2], 1, 0);
            set_bit(&(structures->instructions_array)[locations->IC+2], 2, 1); 
        }
        else if(destination_operand_method==1)
        {
                operand=destination_operand;
                addLabelNode(&structures->label_list, operand, locations->IC+2);
        }
    }

    locations->IC = locations->IC+L;
    node->size = L;
    free_strings(2,destination_operand,source_operand);
    free_strings(2,des,sur);
    return node;


}


void code_operand_method(int first_bit, int opernad_method, DC_IC *locations, date_structures *structures, struct symbols_linked_list *list, char *opernad) {
    /*ARE*/
    set_bit(&(structures->instructions_array)[locations->IC],0 , 0);
    set_bit(&(structures->instructions_array)[locations->IC], 1, 0);
    set_bit(&(structures->instructions_array)[locations->IC],  2, 1);
    if (opernad_method == IMMEDIATE/* || opernad_method == NO_OPPERAND*/) {
        set_bit(&(structures->instructions_array)[locations->IC], first_bit, 0);
        set_bit(&(structures->instructions_array)[locations->IC], first_bit-1, 0);
        set_bit(&(structures->instructions_array)[locations->IC], first_bit-2, 0);
        set_bit(&(structures->instructions_array)[locations->IC], first_bit-3, 1);
    } else if (opernad_method == DIRECT) {
                    set_bit(&(structures->instructions_array)[locations->IC],first_bit , 0);
                    set_bit(&(structures->instructions_array)[locations->IC], first_bit-  1, 0);
                    set_bit(&(structures->instructions_array)[locations->IC],  first_bit- 2, 1);
                    set_bit(&(structures->instructions_array)[locations->IC],  first_bit- 3, 0);
    } else if (opernad_method == INDIRECT) {
        set_bit(&(structures->instructions_array)[locations->IC], first_bit, 0);
        set_bit(&(structures->instructions_array)[locations->IC], first_bit - 1, 1);
        set_bit(&(structures->instructions_array)[locations->IC], first_bit - 2, 0);
        set_bit(&(structures->instructions_array)[locations->IC],  first_bit- 3, 0);

    } else if (opernad_method == REGISTER) {
        set_bit(&(structures->instructions_array)[locations->IC],first_bit, 1);
        set_bit(&(structures->instructions_array)[locations->IC], first_bit-1, 0);
        set_bit(&(structures->instructions_array)[locations->IC],first_bit-2, 0);
         set_bit(&(structures->instructions_array)[locations->IC],first_bit-3, 0);
    }
}


/*Add the IC +100 to the values in the symbol table that are both relocatable and data. and add 100 to the values that are both relocatable and code. (In other words, we will not change the value of the defines and externals at all...)*/
void end_of_pass_1(DC_IC * locations,date_structures * structures,struct file_status * file,FILE * file_am){
	struct symbols_node * current = structures->symbols_list->head;
         /*100 is for the Operating System*/
	/* Traverse the linked list*/
    	while (current != NULL) {
        	if (current->location == DATA && current->update_attribute== RELOCATEABLE) {
			current->value=current->value +100 +locations->IC;
            printf("current->value %d\n",current->value);//DEBUG
        	}
		else if (current->location == CODE && current->update_attribute== RELOCATEABLE){
			current->value=current->value +100;
		}
        	current = current->next; /* Move to the next node*/
        }
}
        
void entries(struct file_status * file,FILE * file_am, date_structures * structures ,DC_IC * locations){
    int i;
    file->line=0; 
    fseek(file_am, 0, SEEK_SET);
    struct symbols_node* fictive = NULL;
    int found_EOF=FALSE;
    char  *buffer2 ,*copy;
    char ** ptp;
    buffer2 = (char *)malloc((MAX_LINE_LENGTH) * sizeof(char));
    while (!found_EOF)
	{
        file->line++;
        buffer2 = (char *)malloc((MAX_LINE_LENGTH) * sizeof(char));
		found_EOF=fill_and_check(buffer2,file_am);
         if (feof(file_am)){
            found_EOF = TRUE;
         }
             copy = buffer2;
		     ptp = &copy;
             fictive=send_to_function_pass_two(ptp,file,structures);
             free(fictive);
    }
            print_list(structures->symbols_list);//DEBUG
    free(buffer2);
    free(structures);
}

struct symbols_node* send_to_function_pass_two(char ** ptp,struct file_status * file, date_structures * structures ){
    char * first_word = NULL;
			first_word =(char*)malloc((MAX_NAME_LENGTH)*sizeof(char));
            first_word = next_word(ptp);
    int x;
            if (!strcmp(first_word,".entry")){
				first_word = next_word(ptp);
                first_word=*ptp;
				x=set_entry(structures->symbols_list, first_word, file);
			}
            free(first_word);
            free(ptp);
}


int compare_strings(const char *str1, const char *str2) {
    while (*str1 && *str2) {
        while (*str1 && !isalpha(*str1)) {
            str1++;
        }
        while (*str2 && !isalpha(*str2)) {
            str2++;
        }
        if (tolower(*str1) != tolower(*str2)) {
            return 0; 
        }
        if (*str1) str1++;
        if (*str2) str2++;
    }
    return *str1 == '\0' && *str2 == '\0';
}


void printBinary15Bit(int n) {
    int i,mask;
    for (i = 14; i >= 0; i--) { 
        mask = 1 << i;
        printf("%d", (n & mask) ? 1 : 0);
    }
    printf("\n");
}