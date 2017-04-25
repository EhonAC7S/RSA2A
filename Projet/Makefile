all: proxy_multiclient proxy_https clear

ok: proxy_v1 clear

proxy_v1: proxy_v1.o utils/util.o utils/socketutil.o utils/patternSearch.o
	gcc -Wall -o proxy_v1 proxy_v1.o utils/util.o utils/socketutil.o utils/patternSearch.o

proxy_v1.o:	proxy_v1.c ./utils/*.h
	gcc -Wall -c proxy_v1.c ./utils/*.h

proxy_multiclient: proxy_multiclient.o utils/util.o utils/socketutil.o utils/patternSearch.o
	gcc -Wall -o proxy_multiclient proxy_multiclient.o utils/util.o utils/socketutil.o utils/patternSearch.o

proxy_https: proxy_https.o utils/util.o utils/socketutil.o utils/patternSearch.o
	gcc -Wall -o proxy_https proxy_https.o utils/util.o utils/socketutil.o utils/patternSearch.o

proxy_https.o:	proxy_https.c ./utils/*.h
	gcc -Wall -c proxy_https.c ./utils/*.h

proxy_multiclient.o:	proxy_multiclient.c ./utils/*.h
	gcc -Wall  -c proxy_multiclient.c ./utils/*.h

socketutil.o: socketutil.c utils/*.h
	gcc -Wall -c socketutil.c utils/*.h

patternSearch.o: patternSearch.c utils/*.h
	gcc -Wall -c patternSearch.c utils/*.h

util.o: utils/util.c utils/*.h
	gcc -Wall -c utils/util.c utils/*.h

clear:
	rm -rf *.o utils/*.o *.gch utils/*.gch
