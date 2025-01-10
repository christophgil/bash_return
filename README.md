# bash_return

# Summary

This  Bash extension written in C allows script functions to return numeric or string values.

Status: Experimental. Successfully Tested:

 - Nested function calls
 - Recursive function calls

Currently, two methods are used to return results from functions, they are either  error prone and verbose or lead to poor performance, respectively,
see  [Details](./motivation.md).

Both problems are efficiently solved with this Bash extension.

# Installation

First [Install dependencies](./INSTALL_DEPENDENCIES.md)

Compile:

    compile_C.sh  bashbuiltin_set_retval.c

# Usage

The compiled builtins must be loaded once.

    enable -f /home/cgille/compiled/bashbuiltin_set_retval.so   init_retval set_retval

The Bash builtin *init_retval* must be called at the top of the function body.
The result of the function is set  with the builtin *set_retval*.

    square(){
        init_retval
        : Do something
        set_retval $(($1*$1))
    }

When the function is called with the leading option -$, then the returned value is obtained from a
variable whose name is stored in the global variable *RETVAL*.  This is possible because Bash is not
multi-threaded. Normally, dollar signs need to be quoted in UNIX shells. Note that the dollar sign does not need to be quoted here because it is followed by
a space. An unusual option character has been chosen to not interfere with other options.

    square -$  3
    echo "The Square of 3 is ${RETVAL}"

Without the option *-$*, the result is directly printed to the standard output.

    square 3

# Example with Arrays

    my_seq(){
        init_retval
        local int n=$1
        local aa=()
        for((i=0;i<n;i++)); do
            aa+=( $((i*i)) )
        done
        set_retval "${aa[@]}"
    }
    my_seq 10


To capture these numbers in an array variable, call it as follows.

    my_seq -$ 10
    numbers="${RETVAL[@]}"
    echo "numbers: ${numbers[@]}"

# Benchmarks

We are doing 40000 function calls with the script *benchmark_set_retval.sh*

    classical      51 Seconds
    Novel method  1.2 Seconds
