/* Author: Christoph Gille   Licence: GNU */
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include "bash/loadables.h"
#define ANSI_RED "\x1B[41m"
#define ANSI_RESET "\x1B[0m"
#define ANSI_FG_BLUE "\x1B[34;1m"

#define FUNCNAME_PFX "RETURNED_FROM_"
#define DEBUG 0

#define FUNCNAME_ADD_PFX (1<<1)
static bool get_function_name(const int flags, char *n,const int max_size){
  if (flags&FUNCNAME_ADD_PFX) n=stpcpy(n,FUNCNAME_PFX);
  *n=0;
  ARRAY *a=NULL;
  SHELL_VAR *v=NULL;
  GET_ARRAY_FROM_VAR("FUNCNAME",v,a);
  if (!a){
    fprintf(stderr,ANSI_RED"No array 'FUNCNAME'"ANSI_RESET"\n");
    return false;
  }else{
    char *a0=array_reference(a,0);
    if (a0){
      if (max_size<=strlen(a0)+(sizeof(FUNCNAME_PFX)-1))  fprintf(stderr,ANSI_RED"function name too long: %ld"ANSI_RESET"\n",(long)strlen(a0));
      else strcpy(n,a0);
    }else{
      fprintf(stderr,ANSI_RED"Array 'FUNCNAME' is empty"ANSI_RESET"\n");
    }
  }
  return true;
}
static void report_error_in_function(){
  char funcname[256];
  get_function_name(0,funcname,sizeof(funcname)-1);
  fprintf(stderr,ANSI_RED"Error "ANSI_RESET" in function "ANSI_FG_BLUE"%s()"ANSI_RESET": ",funcname);
}
int init_retval_builtin(WORD_LIST *list){
  const bool is_retval=posparam_count  && !strcmp(dollar_vars[1],"-$");
  if (DEBUG) fprintf(stderr,"This is init_retval_builtin. Has -$: %s  \n",is_retval?"Yes":"No");
  if (is_retval) shift_args(1);
  char vname[256];
  if (get_function_name(FUNCNAME_ADD_PFX,vname,sizeof(vname))) bind_global_variable(vname,NULL,0);
  make_local_variable("__return_var__",0);
  bind_variable("__return_var__",is_retval?vname:"",0);  /* Either "" or FUNCNAME_PFX$FUNCNAME */
  return EXECUTION_SUCCESS;
}
char *init_retval_doc[]={
  "Initializes returning values to variable.",
  "Must be first statement in function body.",
  "Called with no arguments.",
  "",
  "Internally:",
  " - Shifts away a leading -$ option",
  " - sets shell variable __return_var__ to RETURNED_FROM_$FUNCNAME.",
  (char*)NULL
};


struct builtin init_retval_struct={"init_retval",init_retval_builtin,BUILTIN_ENABLED,init_retval_doc,"init_retval",0};


int set_retval_builtin(WORD_LIST *list){
  if (DEBUG) fprintf(stderr,"This is set_retval_builtin\n");
  SHELL_VAR *v=find_variable("__return_var__");
  if (!v || !local_p(v)){
    report_error_in_function();
    fputs("set_retval without previous init_retval\n",stderr);
  }else{
    char *n=v->value; /* The name of the variable which will hold the return value */
    if (DEBUG) fprintf(stderr,"Variable __return_var__ is %s\n",n);
    if (n && *n){
      const bool is_array=list && list->next && list->next->next; /* more than one arg */
      SHELL_VAR *v=bind_global_variable(n,!is_array && list && list->word?list->word->word:NULL,0);
      if (is_array) assign_array_var_from_word_list(convert_var_to_array(v),list,0);
    }else{
      for(;list;list=list->next){
        char *val=list->word->word;
        puts(val?val:"");
      }
    }
    bind_global_variable("RETVAL",n?n:NULL,0);
  }
  return (EXECUTION_SUCCESS);
}
char *set_retval_doc[]={
  "Sets the return value of a function.",
  "Argument: The  string to be returned.",
  "Usually placed before a return statement or at the end of the function body",
  (char*)NULL
};

struct builtin set_retval_struct={"set_retval",set_retval_builtin,BUILTIN_ENABLED,set_retval_doc,"set_retval",0};

/* Is this OK?   Memory leaks? */

int retval_to_array_builtin(WORD_LIST *list){
  if (!list || !list->word->word){
    report_error_in_function(); fprintf(stderr,"  retval_to_array - missing parameter array-name\n");
    return EXECUTION_FAILURE;
  }
  char *array_name=list->word->word;
  SHELL_VAR *v=find_variable(array_name);
  if (!v){
    fprintf(stderr,"retval_to_array "ANSI_FG_BLUE"%s"ANSI_RESET"   no such variable.\n",array_name);
    return EXECUTION_FAILURE;
  }
  SHELL_VAR *r=find_variable("RETVAL");
  if (!r || !r->value){
    fprintf(stderr,"retval_to_array "ANSI_FG_BLUE"%s"ANSI_RESET":    RETVAL not defined.\n",array_name);
    return EXECUTION_FAILURE;
  }
  SHELL_VAR* src=find_variable(r->value); /* The array defined in the previously called function  */
  // see copy_variable() in /home/_cache/cgille/build/bash-5.2.37/variables.c
  SHELL_VAR* dst=find_variable(array_name); /* The array to be assigned */
  if (!dst || !array_p(dst)){
    fprintf(stderr,"retval_to_array "ANSI_FG_BLUE"%s"ANSI_RESET":    Not an array.\n",array_name);
    return EXECUTION_FAILURE;
  }
  if (array_p(src)){
    array_dispose(array_cell(dst));    /* Free memory of current array */
    dst->value=src->value;          /* Array component copied from src */
    src->value=(char*)array_create(); /* When src is eventually disposed it should not have a reference to the array component. */
  }else{
    array_insert(array_cell(dst),0,src->value);
  }
  return EXECUTION_SUCCESS;
}
char *retval_to_array_doc[]={
  "Sets.",
  (char*)NULL
};
struct builtin retval_to_array_struct={"retval_to_array",retval_to_array_builtin,BUILTIN_ENABLED,retval_to_array_doc,"retval_to_array",0};
