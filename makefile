MASTER_DEPS=m2sADT.c helpers.c slave-controller.c
SLAVE_DEPS=md5sumADT.c helpers.c
STD=gnu11
FLAGS=-g -pedantic -lrt -pthread

all: clean master slave view test_shm

master:
	gcc -Wall master.c $(MASTER_DEPS) -o md5 -std=$(STD) $(FLAGS)

slave:
	gcc -Wall slave.c $(SLAVE_DEPS) -o slave -std=$(STD) $(FLAGS)

view:
	gcc -Wall view.c -o view -std=$(STD) $(FLAGS)

test_shm:
	gcc -Wall test_shm.c -o test_shm -std=$(STD) $(FLAGS)

clean:
	rm -f md5 slave view test_shm