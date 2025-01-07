#!/usr/bin/env bash
# ~/c/bash/bashbuiltin_cg_return.c
bb='cg_return_init cg_return'
enable -d  $bb 2>/dev/null; enable -f /home/cgille/compiled/bashbuiltin_cg_return.so $bb
:
my_func(){
    cg_return_init
    : Do something
    cg_return "hello $1"
}
:
echo "--- Testing printing return value to  stdout ---"
my_func "Parameter for stdout"
echo

echo "--- Testing assign  return value to variable ---"
my_func -@ "Parameter for variable assignment"
echo " LAST_RETURN: ${!LAST_RETURN} "
echo
:
echo "--- Testing nested ---"
my_func_outer(){
    cg_return_init
    my_func -@ inner
    local from_inner=${!LAST_RETURN}
    cg_return "From outer: hello Got from inner: $from_inner"
}
my_func_outer -@ "para for outer"
echo " LAST_RETURN: ${!LAST_RETURN} "
echo
:
echo "--- Testing recursion ---"
faculty(){
    cg_return_init
    local n=$1
    if ((n==1 || n==0)); then
        cg_return 1
    else
        faculty -@ $((n-1))
        local f1=${!LAST_RETURN}
        cg_return $((n*f1))
    fi
}
n=5
echo " Faculty $n"
faculty -@ $n
echo " faculty $n: ${!LAST_RETURN} "
