all: consumer producer fifo

consumer: consumer.c wrappers.h
	gcc consumer.c -o consumer

producer: producer.c wrappers.h
	gcc producer.c -o producer

fifo: fifo.c wrappers.h
	gcc fifo.c -o fifo

.PHONY: all clean

clean:
	rm -f producer consumer fifo
