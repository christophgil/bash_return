#!/usr/bin/env bash
enable -f ~/compiled/bashbuiltin_cg_return.so  init_retval set_retval retval_to_array
sequence(){
    init_retval
    local int n=$1
    local aa=()
    for((i=0;i<n;i++)); do
        aa+=( $((i*i)) )
    done
    set_retval "${aa[@]}"
}
sequence -$ 10

sequence -$ 1
echo "11111111111111111111111 ${!RETVAL:-}"
aa=(Should be overwritten and not seen In result); retval_to_array aa
echo aa ${aa[@]}
