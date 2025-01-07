# bash_return

# Summary

bash_return is a bashbuiltin which allows to return values from functions via variables.

Status: Experimental. Successfully Tested:

 - Nested function calls
 - Recursive function calls

# Usage

First compile:

    compile_C.sh  bashbuiltin_cg_return.c

The compiled builtins must be loaded once.

    enable -f /home/cgille/compiled/bashbuiltin_cg_return.so cg_return_init cg_return

The Bash builtin /cg_return_init/ must be called at the top of the function body.
The result is returned with the builtin  /cg_return/.

    square(){
        cg_return_init
        : Do something
        cg_return $(($1*$1))
    }

When the function is called with the leading option -@, then the returned value is obtained from a
variable whose name is stored in the global variable LAST_RETURN.  This is possible because Bash is
not multi-threaded.

    square -@  3
    echo "Square of 3 is: ${!LAST_RETURN}"

Without the option /-@/, the result is directly printed to the standard output.

    square 3



# Motivation

The ‘return’ command in Bash has certain limitations. It can only return an integer between 0 and 255, and it can only return one value.
Usually, the EXIT_SUCCESS or the error code is returned.



Strings or multiple values are usually  returned by sending them to the output stream, see
Section /Returning Strings/ in https://ioflood.com/blog/bash-function-return-value

    function square() {
        echo $(($1*$1))
    }
    result=$(square 3)

The problem is that this method is very slow.

Alternative strategies have been  discussed to improve performance: https://stackoverflow.com/questions/3236871/how-to-return-a-string-value-from-a-bash-function

One solution works with name references  following:

    function square() {
        declare -n RETURN_FROM_square=$1
        shift
        RETURN_FROM_square==$1=$(($1*$1))
    }
    square  value 3
    echo $value

The code correctly prints the square of the number 3.

The disadvantages are:

  - The body of the function must not define a local variable identical to the return variable
    defined by the caller. The following line inside the body of the function  will break the code.

        local value=8

  - The code is verbose.
  - Code checking tools like shellcheck report  false warnings:

     + warning: RETURN_from_my_func appears unused. Verify use (or export if used externally). [SC2034]
     + warning: value is referenced but not assigned. [SC2154]




# Benchmarks

We are doing 40000 function calls with the script /benchmark_cg_return.sh/

    classical      51 Seconds
    Novel method  1.2 Seconds
