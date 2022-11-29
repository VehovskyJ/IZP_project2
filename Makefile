.PHONY : clean run test

cluster: cluster.c
        gcc -std=c99 -Wall -Wextra -Werror -DNDEBUG cluster.c -o cluster -lm

clean:
        rm -f cluster

run: cluster
        ./cluster seznam.txt 5

test: cluster
        python3 test.py cluster --valgrind