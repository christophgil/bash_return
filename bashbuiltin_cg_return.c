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

static  bool get_function_name(char *n,const int max_size){
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
      if (strlen(a0)>=max_size-sizeof(FUNCNAME_PFX))  fprintf(stderr,ANSI_RED"function name too long: %ld"ANSI_RESET"\n",(long)strlen(a0));
      else strcpy(n,a0);
    }
  }
  return true;
}
static void report_error_in_function(){
  char funcname[256];
  get_function_name(funcname,sizeof(funcname)-1);
  fprintf(stderr,ANSI_RED"Error "ANSI_RESET" in function "ANSI_FG_BLUE"%s()"ANSI_RESET": ",funcname);
}
int init_retval_builtin(WORD_LIST *list){
  if (DEBUG) fprintf(stderr,"This is init_retval_builtin  with -$\n");
  make_local_variable("__return_var__",0);
  char vname[256]; *vname=0;
  if (posparam_count  && !strcmp(dollar_vars[1],"-$")){
    bind_variable("__return_var__","",0);
    shift_args(1);
    char funcname[256];
    if (get_function_name(funcname,sizeof(funcname)-1))  strcpy(stpcpy(vname,FUNCNAME_PFX),funcname);
  }
  bind_variable("__return_var__",vname,0);  /* Either "" or FUNCNAME_PFX$FUNCNAME */
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
  char *val=list && list->word?list->word->word:NULL;
  SHELL_VAR *v=find_variable("__return_var__");
  if (!v || !local_p(v)){
    report_error_in_function();
    fputs("set_retval without previous init_retval\n",stderr);
  }else{
    char *n=v->value;
    if (DEBUG) fprintf(stderr,"Variable __return_var__ is %s\n",n);
    if (n && *n){
      bind_global_variable(n,val?val:NULL,0);
      bind_variable("RETVAL",n,0);
    }else{
      if (val) puts(val);
    }
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
