all: solution.o tree_exec.o token.o recursive_descent.o
	gcc solution.o tree_exec.o token.o recursive_descent.o -o solution


tree_exec.o: tree_exec.c tree_exec.h
	gcc tree_exec.c -c

token.o: token.c token.h
	gcc token.c -c

recursive_descent.o: recursive_descent.c recursive_descent.h
	gcc recursive_descent.c -c 

solution.o: solution.c solution.h
	gcc solution.c -c 



clean:
	rm -f recursive_descent.o solution.o token.o tree_exec.o


	