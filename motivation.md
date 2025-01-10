# Motivation

The *return* command in Bash is usually used to return the exit status. This is an integers between 0 and 255, where 0 stands  for success.
Unlike in other programming languages, It  is usually not used to pass strings or numeric values from the function to the caller.


Strings or numeric values are often  printed to standard output by function. The caller can easily capture the standard output. See
section *Returning Strings* in https://ioflood.com/blog/bash-function-return-value

    square() {
        echo $(($1*$1))
    }
    echo "The square of 3 is $(square 3)"

This looks nice and  compact. However, it wastes about a millisecond which leads to long running loops.
Furthermore, a potential source of problems  are  messages written by commands. Unintentionally, they might get captured as well.
A redirection to stderr avoids this problem:


    my_function() {
       {
          do something
        } >&2
        echo result
    }




The following is much faster:

    square() {
       RETVAL=$(($1*$1))
    }
    square  value 3;  s=$RETVAL
    echo "The square of 3 is $s"

The code correctly prints the square of the number 3. It is slightly more verbose.
In functions with several return statements the programmer must not forget to assign the variable RETVAL.
Calling *unset RETVAL* together with *set -u* may help to recognize such bugs.


It would be nice to have the choice between both notions.  The variable assignment could be used in
loops when performance matters. Otherwise the convenient command substitution could be employed.


The Bash  extension allows to choose between both notions. Command substitution is the default.
Variable assignment is selected with a command line switch.
