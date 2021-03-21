./bin/project3  tests/test1.bc > out.txt
if [ -z $(grep "Reachable" out.txt) ]; then
    echo "test1 error"
fi

./bin/project3  tests/test2.bc > out.txt
if [ -z $(grep "Reachable" out.txt) ]; then
    echo "test2 error"
fi

