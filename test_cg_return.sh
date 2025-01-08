#!/usr/bin/env bash
enable -f ~/compiled/bashbuiltin_cg_return.so  init_retval set_retval

__return_var__="This global var does not interfere with the local var with same name created by init_retval"
:
hello_world(){
    init_retval
    : Do something
    set_retval "hello $1"
}
echo ------------------------------------------------------------
echo "--- Testing printing return value to  stdout ---"
hello_world 123
echo
echo ------------------------------------------------------------
echo "--- Testing assign  return value to variable ---"
hello_world -$ 123
echo " RETVAL: ${!RETVAL} "
echo
echo ------------------------------------------------------------
echo "--- Testing nested function calls ---"
sum(){
    init_retval
    local sum=0 n
    for n in "$@"; do
        ((sum+=n))
    done
    set_retval $sum
}
square(){
    init_retval
    set_retval $(($1*$1))
}

sum_of_squares(){
    init_retval
    local list=() n
    for n in "$@"; do
        square -$ $n
        list+=(${!RETVAL})
    done
    sum -$ "${list[@]}"
    set_retval ${!RETVAL}
}

sum_of_squares -$ 3 4
echo " 3*3 + 4*4 is ${!RETVAL} "
echo
echo ------------------------------------------------------------
echo "--- Testing recursion ---"
faculty(){
    init_retval
    local n=$1
    if ((n==1 || n==0)); then
        set_retval 1
        return 0
    fi
    faculty -$ $((n-1))
    local f1=${!RETVAL}
    set_retval $((n*f1))
}
n=5
faculty -$ $n
echo " faculty of $n is ${!RETVAL} "
echo
echo ------------------------------------------------------------
echo "--- Testing Not parameter ---"

sum -$
echo "The sum of nothing is ${!RETVAL}"
sum
echo
echo ------------------------------------------------------------
echo "--- Test missing init_retval ---"
my_func_forgot_init(){
    set_retval "Hello"
}
my_func_forgot_init -$
echo "my_func_forgot_init:  ${!RETVAL}"
