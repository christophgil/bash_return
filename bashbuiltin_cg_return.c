/* Author: Christoph Gille   Licence: GNU */
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include "bash/loadables.h"
#define ANSI_RED "\x1B[41m"
#define ANSI_RESET "\x1B[0m"
#define ANSI_FG_BLUE "\x1B[34;1m"
#define RED_ERROR ANSI_RED" Error "ANSI_RESET
#define FUNCNAME_PFX "RETURNED_FROM_"
#define DEBUG 0

#define VARNAME_MAX 256

#define RETURN_FAIL() { if (!interactive_shell) exit_shell(EXECUTION_FAILURE); return EXECUTION_FAILURE;}


static bool get_function_name(char *n){ // Can funcname be obtained more easily ?
  n=stpcpy(n,FUNCNAME_PFX);
  ARRAY *a=NULL;
  SHELL_VAR *v=NULL;
  GET_ARRAY_FROM_VAR("FUNCNAME",v,a);
  if (!a){
    report_error(RED_ERROR"No array 'FUNCNAME'\n");
    return false;
  }else{
    const char *a0=array_reference(a,0);
    if (a0){
      if (VARNAME_MAX<=sizeof(FUNCNAME_PFX)-1+strlen(a0))  report_error(RED_ERROR"function name too long: %s\n",a0);
      else strcpy(n,a0);
    }else{
      report_error(RED_ERROR"Array 'FUNCNAME' is empty\n");
    }
  }
  return true;
}
int init_retval_builtin(WORD_LIST *list){
  const bool is_retval=posparam_count>0 && !strcmp(dollar_vars[1],"-$");
  if (DEBUG) fprintf(stderr,"This is init_retval_builtin. Has -$: %s  \n",is_retval?"Yes":"No");
  if (is_retval) shift_args(1);
  bind_variable_value(make_local_variable("__return_var__",0),strdup(is_retval?"1":"0"),0);  /* Either "" or FUNCNAME_PFX$FUNCNAME */  // ??? strdup
  {
    /* In case of forgotten set_retval - clear previous variable content  */
    char vname[VARNAME_MAX]; *vname=0;
    if (get_function_name(vname)){
      bind_global_variable(vname,NULL,0);
      //if (v) bind_variable_value(v,NULL,0); // ??? The old value is properly freed? Or do I need to take care of this?
    }
  }
  return EXECUTION_SUCCESS;
}
char *init_retval_doc[]={
  "Initializes returning results.",
  "Must be first statement in function body.",
  "No arguments.",
  "",
  "Internally:",
  " - Shifts away a leading -$ option",
  " - sets shell variable __return_var__ to 'RETURNED_FROM_$FUNCNAME'",
  (char*)NULL
};

struct builtin init_retval_struct={"init_retval",init_retval_builtin,BUILTIN_ENABLED,init_retval_doc,"init_retval",0};
int set_retval_builtin(WORD_LIST *list){
  if (DEBUG) fprintf(stderr,"This is set_retval_builtin\n");
  bool is_stdout=true;
  {
    const SHELL_VAR *v=find_variable("__return_var__");
    if (!v || !local_p(v)){
      report_error(RED_ERROR"set_retval without previous init_retval\n");
      RETURN_FAIL();
    }else{
      if (v->value && *v->value=='1') is_stdout=false;
    }
  }
  if (DEBUG) fprintf(stderr,"Variable __return_var__ is %d\n",is_stdout);
  char vname[VARNAME_MAX]; *vname=0;
  if (!is_stdout){
    if (get_function_name(vname)){
      assign_array_var_from_word_list(convert_var_to_array(bind_global_variable(vname,NULL,0)),list,0);
    }
  }else{
    for(;list;list=list->next){
      const char *val=list->word->word;
      puts(val?val:"");
    }
  }
  //  const char *s=strdup(vname);
  char *s=vname; /* Why not const char?   Is it freed?  */
  SHELL_VAR *sv=bind_global_variable("RETVAL",s,0);
  assert(sv->value!=s); /* Sounds like I do not need stdup ???? */


  return EXECUTION_SUCCESS;
}
char *set_retval_doc[]={
  "Sets the return value of a function.",
  "Argument: The  string to be returned.",
  "Usually placed before a return statement or at the end of the function body",
  (char*)NULL
};

struct builtin set_retval_struct={"set_retval",set_retval_builtin,BUILTIN_ENABLED,set_retval_doc,"set_retval",0};

/* Is this OK?   Memory leaks? */

#define E RED_ERROR"retval_to_array "ANSI_FG_BLUE"%s"ANSI_RESET"  "
int retval_to_array_builtin(WORD_LIST *list){
  if (!list || !list->word || !list->word->word){
    report_error(E"Missing parameter array-name\n","");
    RETURN_FAIL();
  }
  const char *array_name=list->word->word;
  SHELL_VAR* dst=find_variable(array_name); /* The array to be assigned */
  if (!dst){
    report_error(E"no such variable.\n",array_name);
    RETURN_FAIL();
  }
  const SHELL_VAR *r=find_variable("RETVAL");
  if (!r || !r->value){
    report_error(E"RETVAL not defined.\n","");
    RETURN_FAIL();
  }
  SHELL_VAR* src=find_variable(r->value); /* The array defined in the previously called function  */
  // see copy_variable() in /home/_cache/cgille/build/bash-5.2.37/variables.c
  if (!array_p(dst)){
    report_error(E"Not an array.\n",array_name);
        RETURN_FAIL();
  }
  if (array_p(src)){
    array_dispose(array_cell(dst));     /* Free memory of current array */ // Is this OK
    dst->value=src->value;              /* Array component copied from src */
    src->value=NULL;                    /* When src is eventually disposed it should not have a reference to the array. */
  }else{
    array_insert(array_cell(dst),0,src->value);
  }
  return EXECUTION_SUCCESS;
}
#undef E
char *retval_to_array_doc[]={
  "After calling a function Assigning the result to an array.",
  "Parameter: The name of the existing array to be assigned.",
  "Example:  ",
  "   local aa(); retval_to_array aa",
  (char*)NULL
};
struct builtin retval_to_array_struct={"retval_to_array",retval_to_array_builtin,BUILTIN_ENABLED,retval_to_array_doc,"retval_to_array",0};
