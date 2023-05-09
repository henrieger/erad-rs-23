CC = mpicc
CFLAGS = -Wall -O6 -mavx
LDLIBS = 

target = knn
objects = verificaKNN.o

size_p = 20
size_q = 16
sample = 4
dimensions = 3

all: $(target)

$(target): knn.c $(objects)
verificaKNN.o: verificaKNN.c verificaKNN.h

knn_seq: knn_seq.c $(objects)

run_seq:
	./knn_seq $(size_p) $(size_q) $(sample) $(dimensions)
run_mpi_seq:
	mpirun -np 1 ./$(target) $(size_p) $(size_q) $(sample) $(dimensions)
run_local:
	mpirun -np 4 ./$(target) $(size_p) $(size_q) $(sample) $(dimensions)
run_cluster:
	mpirun --hostfile hostfile.txt ./$(target) $(size_p) $(size_q) $(sample) $(dimensions) 2> /dev/null

debug: CFLAGS += -g -DDEBUG
debug: all

verifica: CFLAGS += -DVERIFICA
verifica: all

clean:
	rm -f $(objects)
purge: clean
	rm -f $(target)
