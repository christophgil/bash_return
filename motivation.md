# Motivation

The ‘return’ command in Bash has certain limitations. It can only return an integer between 0 and 255, and it can only return one value.
Usually, the EXIT_SUCCESS or the error code is returned.



Strings or multiple values are usually  returned by sending them to the output stream, see
Section /Returning Strings/ in https://ioflood.com/blog/bash-function-return-value

    function square() {
        echo $(($1*$1))
    }
    result=$(square 3)
    echo "The square of 3 is $value"

The problem is that this method is very slow.

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

     + warning: RETURN_from_my_func appears unused. Verify use (or export if used externally). [SC2034]
     + warning: value is referenced but not assigned. [SC2154]

Apparently, it is not easily possible to get a satisfactory solution at shell script level.
Therefore the bashbuiltin has been developed.
