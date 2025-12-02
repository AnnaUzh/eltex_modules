all:
	gcc -shared -fPIC -o add.so add.c
	gcc -shared -fPIC -o subtract.so subtract.c
	gcc -shared -fPIC -o multiply.so multiply.c
	gcc -shared -fPIC -o divide.so divide.c
	gcc -shared -fPIC -o power.so power.c -lm
	gcc -o calculator main.c -ldl

clean:
	rm -f *.so calculator

run: all
	./calculator