build:
	g++ -o main.out -g main.cpp
	./main.out

test:
	nasm -o object.o -f elf32 object.asm
	g++ -o test.o -m32 -c test.cpp
	g++ -o test.out test.o object.o -m32
	./test.out

clean:
	rm ./output/*.log *.o *.out