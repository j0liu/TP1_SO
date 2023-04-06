MASTER_DEPS=m2sADT.c helpers.c slave-controller.c
SLAVE_DEPS=md5sumADT.c helpers.c
STD=gnu11

all: clean master slave view

master:
	gcc -Wall master.c $(MASTER_DEPS) -o md5 -std=$(STD) -g -pedantic

slave:
	gcc -Wall slave.c $(SLAVE_DEPS) -o slave -std=$(STD) -g -pedantic

view:
	gcc -Wall view.c -o view -std=$(STD) -g -pedantic

clean:
	rm -f md5 slave view