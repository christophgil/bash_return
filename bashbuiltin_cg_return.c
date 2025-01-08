#include <unistd.h>
#include <stdio.h>
#include "bash/loadables.h"

#define ANSI_RED "\x1B[41m"
#define ANSI_RESET "\x1B[0m"

#define FUNCNAME_PFX "RETURNED_FROM_"


#define DEBUG 0
int init_retval_builtin(WORD_LIST *list){
  char funcname[256]; *funcname=0;
  if (posparam_count  && !strcmp(dollar_vars[1],"-$")){
    if (DEBUG) printf("This is init_retval_builtin  with -$\n");
    make_local_variable("__return_var__",0);
    bind_variable("__return_var__","",0);
    ARRAY *a=NULL;
    SHELL_VAR *v=NULL;
    GET_ARRAY_FROM_VAR("FUNCNAME",v,a);
    if (!a){
      fprintf(stderr,ANSI_RED"No array 'FUNCNAME'"ANSI_RESET"\n");
    }else{
      char *a0=array_reference(a,0);
      if (a0){
        if (strlen(a0)>=sizeof(funcname)-sizeof(FUNCNAME_PFX))  fprintf(stderr,ANSI_RED"function name too long: %ld"ANSI_RESET"\n",strlen(a0));
        else strcpy(funcname,a0);
      }
    }
    shift_args(1);
  }
  if (*funcname){
    char n[256];
    strcpy(stpcpy(n,FUNCNAME_PFX),funcname);
    bind_variable("__return_var__",n,0);
  }
  return (EXECUTION_SUCCESS);
}
char *init_retval_doc[]={
  "Initializes returning values to variable.  .",
  "",
  "Must be first statement in function body. Called with no arguments. Shifts away a leading -$ option and sets shell variable __return_var__ to RETURNED_FROM_$FUNCNAME.",
  (char*)NULL
};
struct builtin init_retval_struct={"init_retval",init_retval_builtin,BUILTIN_ENABLED,init_retval_doc,"init_retval",0};


int set_retval_builtin(WORD_LIST *list){
  if (DEBUG) printf("This is set_retval_builtin\n");
  char *val=list && list->word?list->word->word:NULL;
  SHELL_VAR *v=find_variable("__return_var__");
  if (v && local_p(v)){
    char *n=v->value;
    if (DEBUG) printf("Variable __return_var__ is %s\n",n);
    if (n && *n){
      bind_global_variable(n,val?val:NULL,0);
      bind_variable("RETVAL",n,0);
    }
  }else{
      if (val) puts(val);
    }
  return (EXECUTION_SUCCESS);
}
char *set_retval_doc[]={
  "Return.",
  "",
  "Usually last statement in function body.  Argument: String to be returened.",
  (char*)NULL
};

struct builtin set_retval_struct={"set_retval",set_retval_builtin,BUILTIN_ENABLED,set_retval_doc,"set_retval",0};
