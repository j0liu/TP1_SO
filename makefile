all: master slave view

master:
	gcc -Wall master.c helpers.c -o md5 -std=c99

slave:
	gcc -Wall slave.c helpers.c -o slave -std=c99

view:
	gcc -Wall view.c helpers.c -o view -std=c99

clean:
	rm -f md5 slave view