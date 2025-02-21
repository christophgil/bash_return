/* Author: Christoph Gille   Licence: GNU */
#include "bash/loadables.h"
#include "cg_bashbuiltin.h"
#define DEBUG 0


int init_retval_builtin(WORD_LIST *list){
  const bool is_retval=posparam_count>0 && !strcmp(dollar_vars[1],"-$");
  if (DEBUG) fprintf(stderr,"This is init_retval_builtin. Has -$: %s  \n",is_retval?"Yes":"No");
  if (is_retval) shift_args(1);
  bind_variable_value(make_local_variable("__return_var__",0),strdup(is_retval?"1":"0"),0); // strdup ????
  set_var_retval_invisible();
  return EXECUTION_SUCCESS;
}
char *init_retval_doc[]={
  "Initializes returning results.",
  "Must be first statement in function body.",
  "No arguments.",
  "",
  "Internally:",
  " - Shifts away a leading -$ option",
  " - sets shell variable __return_var__ to 0 or 1",
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
  if (!is_stdout){
      assign_array_var_from_word_list(convert_var_to_array(bind_global_variable(VARNAME_RETVAL,NULL,0)),list,0);
  }else{
    for(;list;list=list->next){
      const char *val=list->word->word;
      puts(val?val:"");
    }
  }
  return EXECUTION_SUCCESS;
}
char *set_retval_doc[]={
  "Sets the return value of a function.",
  "Argument: The  string[s] to be returned.",
  "Placed before each return statement and at the end of the function body",
  (char*)NULL
};

struct builtin set_retval_struct={"set_retval",set_retval_builtin,BUILTIN_ENABLED,set_retval_doc,"set_retval",0};
