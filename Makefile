#Makefile

PROJECT = main.o

CC = g++
#LIB = -lpthread

ab : $(PROJECT)
	$(CC) -g -Wall -o ab $(PROJECT) -lpthread
      
main.o: person.h addrbook.h server.h

clean :
	rm ab $(PROJECT) 


