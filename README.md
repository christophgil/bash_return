# bash_return

# Summary

This  Bash extension written in C allows script functions to return numeric or string values.

Status: Experimental


Generally, two methods can be used to return results from functions to the caller of the function.
(I) Using command substitution to capture the output may cause poor performance.
(II) Storing the result in a global variable is less compact and requires two statements.


With this Bash extension, functions can be used in both ways depending on a command line switch.
See  [Details](./motivation.md).


# Installation

First [Install dependencies](./INSTALL_DEPENDENCIES.md)

Compile:

    compile_C.sh  bashbuiltin_cg_return.c

# Usage

The compiled builtins must be loaded once.

    enable -f /home/cgille/compiled/bashbuiltin_cg_return.so   init_retval set_retval

The Bash builtin *init_retval* must be called at the top of the function body.
The return value of the function is set  with the builtin *set_retval*.

    square(){
        init_retval
        : Do something
        set_retval $(($1*$1))
    }

When the function is called with the switch -$, then the returned value is obtained from a variable
whose name is stored in the global variable *RETVAL*.  Usage of a global variable is possible
because Bash is not multi-threaded. Normally, dollar signs need to be quoted in UNIX shells. Note
that the dollar sign does not need to be quoted here because it is followed by a space. This rather
unusual command line switch is unlikely to be used already.  Note that *-$* must be the first
parameter. It is shifted out by *init_retval* and will not be in the argument list any more.

    square -$  3
    echo "The Square of 3 is ${RETVAL}"

Without the option *-$*, the result is printed to the standard output.

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
    numbers=("${RETVAL[@]}")
    echo "There are ${#numbers[@]} numbers. These are ${numbers[@]}"

# Benchmarks

Performing 40000 function calls with the script *benchmark_set_retval.sh*

    classical      51 Seconds
    Novel method  1.2 Seconds
