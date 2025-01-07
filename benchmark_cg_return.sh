#!/usr/bin/env bash
bb='cg_return_init cg_return'
enable -d  $bb 2>/dev/null; enable -f /home/cgille/compiled/bashbuiltin_cg_return.so $bb
N=40000
case ${1:-X} in
    c) echo "Conventional"
       my_func(){
           : Do something
           echo "The parameter of ${FUNCNAME[0]} is $1"
       }
       for((i=0;i<N;i++)); do
           value=$(my_func $i)
           ((i%100==0)) && echo "i/$N: $i  returned value: $value"
       done
       ;;
    n) echo "Novel methdod"
       my_func(){
           cg_return_init
           : Do something
           cg_return "The parameter of ${FUNCNAME[0]} is $1"
       }
       for((i=0;i<N;i++)); do
           my_func -$ $i
           value=${!LAST_RETURN}
           ((i%100==0)) && echo "i/$N: $i  returned value: $value"
       done
       ;;
    *)
        cat <<EOF
Please run the benchmark in the following way
:
  time $0 c # Conventional

  time $0 n # New via var assignent
EOF
esac
