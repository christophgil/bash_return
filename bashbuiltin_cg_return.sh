#!/usr/bin/env bash
set -u
cg_let(){
    local var_eq_func=$1
    local var=${var_eq_func%=*}  func=${var_eq_func#*=}
   # echo "func=$func var=$var"
    shift
    $func "$@"
#    echo "dddddddddd $var=$RETURNED_FROM_$func"
    local val=RETURNED_FROM_$func
    declare  -g "$var=${!val}"
}


my_func(){
    RETVAL=RETURNED_FROM_$FUNCNAME
    declare -G "$RETVAL=$1"
}

my_func_slow(){
    echo "$@"
}


fast(){
    for((i=0;i<10000;i++)); do
        my_func hello2
       local result=${!RETVAL}
        ((i%1000==0)) && echo "$i RETVAL=$RETVAL result=$result"
    done
}

slow(){
    for((i=0;i<10000;i++)); do
        local  result=$(my_func_slow hello1)
        ((i%1000==0)) && echo $i result=$result
    done
}

fast
echo RESULT=$result
