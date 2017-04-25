all: proxy_v1  clear

ok: proxy_v1 clear

proxy_v1: proxy_v1.o utils/util.o utils/socketutil.o utils/patternSearch.o
	gcc -Wall -o proxy_v1 proxy_v1.o utils/util.o utils/socketutil.o utils/patternSearch.o

proxy_v1.o:	proxy_v1.c ./utils/*.h
	gcc -Wall -c proxy_v1.c ./utils/*.h



socketutil.o: socketutil.c utils/*.h
	gcc -Wall -c socketutil.c utils/*.h

patternSearch.o: patternSearch.c utils/*.h
	gcc -Wall -c patternSearch.c utils/*.h

util.o: utils/util.c utils/*.h
	gcc -Wall -c utils/util.c utils/*.h

secureutil.o: utils/secureutil.o utils/*.h
	gcc -Wall -c utils/secureutil.c utils/*.h -lssl -lcrypto  

clear:
	rm -rf *.o utils/*.o *.gch utils/*.gch
