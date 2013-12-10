CFLAGS=-Wall -Imongoose -Imegahal -pthread -g 

all:    megahal.o mongoose.o
	$(CC) $(CFLAGS) main.c mongoose.o megahal.o -lm -ldl -o megahal-server

mongoose.o: mongoose/mongoose.c mongoose/mongoose.h
	$(CC) -fPIC $(CFLAGS) -include signal.h -std=c99 -O2 -pedantic -pipe -c mongoose/mongoose.c

megahal.o: megahal/megahal.c megahal/megahal.h
	$(CC) -fPIC $(CFLAGS) -c megahal/megahal.c

clean: 
	rm -f *.o megahal-server megahal.txt megahal.log
