
C_FLAGS := -Wall -pedantic -static -g -O0

all: _dir
	gcc $(C_FLAGS) linux_test_main.c ../src/tiny_prompt.c -o ./bin/linux_test

_dir:
	mkdir -p bin

clean:
	rm bin -rf
	
run:
	./bin/linux_test

