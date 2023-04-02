MASTER_DEPS=m2sADT.c helpers.c

all: master slave view

master:
	gcc -Wall master.c $(MASTER_DEPS) -o md5 -std=gnu11 -fsanitize=address -pedantic

slave:
	gcc -Wall slave.c -o slave -std=gnu11 -fsanitize=address -pedantic

view:
	gcc -Wall view.c -o view -std=gnu11 -fsanitize=address -pedantic

clean:
	rm -f md5 slave view