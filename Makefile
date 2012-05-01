all: source sink

source: source.c
	gcc -o source source.c -lxenvchan

sink: sink.c
	gcc -o sink sink.c -lxenvchan

.PHONY:clean
clean:
	rm -f source sink

