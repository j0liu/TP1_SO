MASTER_DEPS=m2sADT.c helpers.c slave-controller.c
STD=c99

all: clean master slave view

master:
	gcc -Wall master.c $(MASTER_DEPS) -o md5 -std=$(STD) -fsanitize=address -pedantic

slave:
	gcc -Wall slave.c -o slave -std=$(STD) -fsanitize=address -pedantic

view:
	gcc -Wall view.c -o view -std=$(STD) -fsanitize=address -pedantic

clean:
	rm -f md5 slave view