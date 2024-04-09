all: cari-ctrl

cari-ctrl: cari-ctrl.c
	gcc -O2 -Wall -Wextra cari-ctrl.c -o cari-ctrl -lzmq -lm

install: all
	install cari-ctrl /usr/local/bin

clean:
	rm -f cari-ctrl