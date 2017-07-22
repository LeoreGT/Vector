CFLAGS = -g -ansi -pedantic -Werror -Wall -I ../../inc/
CC = gcc

#Vector
vector.o: $(VECPATH)vector.c $(INCPATH)vector.h
  $(CC) -g -c -o vector.o $(FLAGS) -fPIC vector.c
