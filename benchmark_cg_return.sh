#!/usr/bin/env bash
bb='init_retval set_retval'
enable -d  $bb 2>/dev/null; enable -f /home/cgille/compiled/bashbuiltin_set_retval.so $bb
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
           init_retval
           : Do something
           set_retval "The parameter of ${FUNCNAME[0]} is $1"
       }
       for((i=0;i<N;i++)); do
           my_func -$ $i
           value=${!RETVAL}
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
