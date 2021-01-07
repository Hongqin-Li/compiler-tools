set -o errexit

TMP=obj/tmp
N=10

make

bench_ret=0
bench() {
  n=$1
  cmd=$2
  t=0
  avg=0
  for i in $(seq $n); do
    \time -f %U $cmd > /dev/null 2> $TMP
    # t=$(time -f %U $cmd 2>&1 1>&3)
    t=$(echo $t+$(tail -n 1 $TMP) | bc -l)
    avg=$(echo $t/$i | bc -l)
    echo "$i: t: $t, avg: $avg"
  done
  bench_ret=$avg
}


echo "- benching C99..."
bench $N "make test-c.y"; echo "- performance of yacc: $bench_ret"
bench $N "make test-c.cc"; echo "- performance of my: $bench_ret"

echo "- benching SysY..."
bench $N "make test-sysy.y"; echo "- performance of yacc: $bench_ret"
bench $N "make test-sysy.cc"; echo "- performance of my: $bench_ret"

echo "- benching Pascal..."
bench $N "make test-pascal.y"; echo "- performance of yacc: $bench_ret"
bench $N "make test-pascal.cc"; echo "- performance of my: $bench_ret"

echo "- benching ECMAScript 1 ..."
bench $N "make test-js.y"; echo "- performance of yacc: $bench_ret"
bench $N "make test-js.cc"; echo "- performance of my: $bench_ret"

