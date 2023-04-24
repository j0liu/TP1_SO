MASTER_DEPS=m2sADT.c helpers.c slave-controller.c resultIOADT.c
SLAVE_DEPS=md5sumADT.c helpers.c
VIEW_DEPS=resultIOADT.c helpers.c
STD=gnu11
FLAGS=-g -pedantic -lrt -pthread

all: clean master slave view view2 output

output:
	mkdir -p output

master:
	gcc -Wall master.c $(MASTER_DEPS) -o md5 -std=$(STD) $(FLAGS)

slave:
	gcc -Wall slave.c $(SLAVE_DEPS) -o slave -std=$(STD) $(FLAGS)

view:
	gcc -Wall view.c $(VIEW_DEPS) -o view -std=$(STD) $(FLAGS)

view2:
	gcc -Wall view2.c $(VIEW_DEPS) -o view2 -std=$(STD) $(FLAGS)

clean:
	rm -f md5 slave view view2

.PHONY: all clean output