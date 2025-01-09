# bash_return

# Summary

This  Bash extension written in C allows script functions to return numeric or string values.

Status: Experimental. Successfully Tested:

 - Nested function calls
 - Recursive function calls



This Bash extension has been developed because the standard way of returning results from script functions often leads to poor performance of scripts.
This well known limitation has been discussed extensively. See  [Details](./motivation.md).

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
    echo "The Square of 3 is ${!RETVAL}"

Without the option *-$*, the result is directly printed to the standard output.

    square 3


# Arrays

[Returning arrays](./arrays.md) is still experimental. Do not use this yet.


# Benchmarks

We are doing 40000 function calls with the script *benchmark_set_retval.sh*

    classical      51 Seconds
    Novel method  1.2 Seconds
