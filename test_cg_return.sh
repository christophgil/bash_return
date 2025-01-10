#!/usr/bin/env bash
set -u
enable -f ~/compiled/bashbuiltin_cg_return.so  init_retval set_retval

readonly ANSI_RED=$'\e[41m'
readonly ANSI_FG_GREEN=$'\e[32m'
readonly ANSI_RESET=$'\e[0m'

assert_eq(){
    local v="$2"
    [[ $v == '@' ]] && v="${RETVAL:-}"
    if [[ $1 == "$v" ]]; then
        echo "${ANSI_FG_GREEN}OK$ANSI_RESET"
    else
        echo "${ANSI_RED}WRONG RESULT$ANSI_RESET in Line ${BASH_LINENO[0]}    '$1' != '$v'"
        read -r -p "Enter"
    fi
}

    sequence(){
        init_retval
        local n=$1
        local aa=()
        for((i=0;i<n;i++)); do
            aa+=( $((i*i)) )
        done
        set_retval "${aa[@]}"
    }


__return_var__="This global var does not interfere with the local var with same name created by init_retval"
:
main(){
    hello_world(){
        init_retval
        : Do something
        set_retval "hello $1"
    }
    echo ------------------------------------------------------------
    echo "--- Testing printing return value to stdout ---"
    hello_world 123
    assert_eq 'hello 123' "$(hello_world 123)"
    echo
    echo ------------------------------------------------------------
    echo "--- Testing assign  return value to variable ---"
    hello_world -$ 123
    echo " RETVAL: ${RETVAL} "
    assert_eq 'hello 123' @
    echo
    echo ------------------------------------------------------------
    echo "--- Testing nested function calls ---"
    sum(){
        init_retval
        local sum=0 n
        for n in "$@"; do
            ((sum+=n))
        done
        set_retval $sum
    }
    square(){
        init_retval
        set_retval $(($1*$1))
    }
    sum_of_squares(){
        init_retval
        local list=() n
        for n in "$@"; do
            square -$ $n
            list+=(${RETVAL})
        done
        sum -$ "${list[@]}"
        set_retval ${RETVAL}
    }

    sum_of_squares -$ 3 4
    echo " 3*3 + 4*4 is ${RETVAL} "
    assert_eq '25' @
    echo
    echo ------------------------------------------------------------
    echo "--- Testing recursion ---"
    faculty(){
        init_retval
        local n=$1
        if ((n==1 || n==0)); then
            set_retval 1
            return 0
        fi
        faculty -$ $((n-1))
        local f1=${RETVAL}
        set_retval $((n*f1))
    }
    local n=5
    faculty -$ $n
    echo " faculty of $n is ${RETVAL} "
    assert_eq '120'  @
    echo
    echo ------------------------------------------------------------
    echo "--- Testing  empty parameter list ---"
    sum -$
    echo "The sum of nothing is ${RETVAL}"
    assert_eq 0 @
    echo
    assert_eq 0 $(sum)
    echo
    $0 1
    echo ------------------------------------------------------------
    echo "--- Test missing set_retval ---"
    echo "There should be no carry over from previous invocation. "
    my_func_forgot_set(){
        init_retval
        [[ -z ${1:-} ]] && return # forgot set_retval
        set_retval "Param is $1"
    }
    my_func_forgot_set -$ 1
    echo "my_func_forgot_set:  ${RETVAL}"
    assert_eq 'Param is 1' @

    my_func_forgot_set -$
    echo "my_func_forgot_set:  ret: '${RETVAL:-}' "
    assert_eq '' @
    #'${RETVAL:-}'
    #   fi
    echo AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
    echo "--- Testing  array stdout ---"
    sequence 10 |tr '\n' '|'
    assert_eq '0|1|4|9|16|25|36|49|64|81|' "$(sequence 10|tr '\n' '|')"
    echo
    echo AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
    echo "--- Testing  array to variable ---"
    sequence -$ 10
    echo
    local aa=("${RETVAL[@]}")
    echo 'This is returned' "${aa[@]}"
    assert_eq '0 1 4 9 16 25 36 49 64 81'  "${aa[*]}"


    echo "Number of values: ${#aa[@]}"
    assert_eq 10  "${#aa[@]}"
    echo

    echo AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
    echo "--- Testing  array to variable only one element ---"
    sequence -$ 1
    local aa=("${RETVAL[@]}")
    echo 'This is returned' "${aa[@]}"
    assert_eq  0  "${aa[*]}"
    assert_eq  1  "${#aa[@]}"


    echo
    echo "${ANSI_FG_GREEN}End of  $0 $ANSI_RESET"
}


with_error1(){
    echo EEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE
    echo "--- Test missing init_retval ---"
    echo "There should be an error report"
    my_func_forgot_init(){
        set_retval "Hello"
        local res=$?
        assert_eq "$res"  '1'
    }
    my_func_forgot_init -$
    echo "${ANSI_RED}This line should not be reached. The shell has already exited.$ANSI_RESET"
}


case ${1:-} in
    1) with_error1;;

    *)        main;;
esac
