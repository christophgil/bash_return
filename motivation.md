# Motivation

The *return* command in Bash is usually used to return *EXIT_SUCCESS* or an  error code from function.
It can only return an integer between 0 and 255, and it can only return one value.


Strings or multiple values are usually returned indirectly. They are printed to standard output in the function. The caller captures the standard output produced by the function. See
section *Returning Strings* in https://ioflood.com/blog/bash-function-return-value

    function square() {
        echo $(($1*$1))
    }
    result=$(square 3)
    echo "The square of 3 is $value"

Despite the compactnes of the code, it wastes about a millisecond.
Calling functions inside a loop causes performance  problems.

Alternative strategies have been  discussed to improve performance: https://stackoverflow.com/questions/3236871/how-to-return-a-string-value-from-a-bash-function
A promising solution involves name references:

    function square() {
        declare -n RETURN_FROM_square=$1
        shift
        RETURN_FROM_square==$1=$(($1*$1))
    }
    square  value 3
    echo "The square of 3 is $value"

The code correctly prints the square of the number 3.

The disadvantages are:

  - The body of the function must not define a local variable identical to the return variable
    defined by the caller. The following line inside the body of the function would break the code.

        local value=8

  - The code is verbose.
  - Code checking tools like shellcheck report  false warnings:

     + RETURN_from_my_func appears unused.
     + value is referenced but not assigned.

Apparently, it is not easily possible to get a satisfactory solution at shell script level.

Therefore the extension  has been implemented as a bash-builtin in C.
