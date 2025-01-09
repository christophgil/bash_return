# Arrays

This is not really validated. Not sure whether this is stable or whether there are memory leaks.
This should be seen by a Bash insider first.

Consider the following function which prints the first n natural numbers.

    my_seq(){
        init_retval
        local int n=$1
        local aa=()
        for((i=0;i<n;i++)); do
            aa+=( $((i*i)) )
        done
        set_retval ${aa[@]}
    }
    my_seq 10



To capture these numbers in an array variable, call it as follows. (Note outside of functions without *local*):


    my_seq 10
    local aa=();  retval_to_array aa

The series of numbers is in the array *aa* and can be printed as follows:

echo ${aa[@]}
