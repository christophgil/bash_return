#!/usr/bin/env bash
enable -f ~/compiled/bashbuiltin_cg_return.so  init_retval set_retval retval_to_array


my_seq(){
    init_retval
    local int n=$1
    local aa=()
    for((i=0;i<n;i++)); do
        aa+=( $((i*i)) )
    done
    set_retval ${aa[@]}
}

echo ------------------------------------------------------------
echo "--- Testing  array stdout ---"
my_seq 10

echo ------------------------------------------------------------
echo "--- Testing  array to variable ---"

my_seq -$ 10

if false; then # This is a Bash-only solution
    typeset -n ref=$RETVAL
    aa=(${ref[@]})
else # This is is using the builtin
    aa=(Element0 Element1); retval_to_array aa
fi
echo This is returned ${aa[@]}



echo ------------------------------------------------------------
echo "--- Testing  array to variable only one element ---"

my_seq -$ 1
aa=(Element0 Element1); retval_to_array aa
echo This is returned ${aa[@]}
