CC = gcc
all = -std=c17 -pedantic -Wall -fPIE

saage: main.o arbres_binaires.o greffe.o saage.o
	${CC} ${all} -o saage main.o arbres_binaires.o greffe.o saage.o

main.o: main.c
	${CC} ${all} -c -o main.o main.c

arbres_binaires.o: arbres_binaires.c 
	${CC} ${all} -c -o arbres_binaires.o arbres_binaires.c
	
greffe.o: greffe.c 
	${CC} ${all} -c -o greffe.o greffe.c

saage.o: saage.c
	${CC} ${all} -c -o saage.o saage.c

