# Nom du compilateur
CC	= gcc

# Options du compilateur
CFLAGS 	= -Wall -pedantic
CFLAGS += -m32
#CFLAGS += -D_XOPEN_SOURCE=500 # Pour prog. système
CFLAGS += -g

all: main
# Liste des cibles du makefile
main : main.o contexte.o hw.o 
	$(CC) $(CFLAGS) -o main main.o contexte.o hw.o

# Listes des fichiers .o nécessaires
main.o : main.c contexte.o 
	$(CC) $(CFLAGS) -c main.c
contexte.o : contexte.c contexte.h
	$(CC) $(CFLAGS) -c contexte.c
hw.o : hw.c
	$(CC) $(CFLAGS) -c hw.c

# Commandes de nettoyage
.PHONY: clean realclean
clean:
	$(RM) *.o *~ main

