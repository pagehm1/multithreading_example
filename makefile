hw4: hw4.c modifyFile.c queue.c
	gcc -g hw4.c modifyFile.c queue.c -lpthread -o hw4

clean:
	rm hw4
