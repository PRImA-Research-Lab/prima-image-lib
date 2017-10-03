CPP=g++
CFLAGS=-fPIC -g -c -Wall -pedantic
INCLUDES=-I../image/
INSTALLLIB=install -m 644 -t /usr/lib/
HEADINSTALLDIR=/usr/include/extra/
INSTALLHEAD=install -m 644 -t ${HEADINSTALLDIR}
HEADERS=Array.h ExtraMath.h ExtraString.h

all: libextra.so

libextra.so: ExtraMath.o ExtraString.o
	${CPP} -shared -Wl,-soname,libextra.so -o libextra.so ExtraMath.o ExtraString.o

ExtraMath.o: ExtraMath.h ExtraMath.cpp
	${CPP} ${CFLAGS} ${INCLUDES} ExtraMath.cpp

ExtraString.o: ExtraString.h ExtraString.cpp
	${CPP} ${CFLAGS} ${INCLUDES} ExtraString.cpp

clean:
	rm -f ExtraMath.o ExtraString.o libextra.so

install:
	${INSTALLLIB} libextra.so
	mkdir -p ${HEADINSTALLDIR}
	${INSTALLHEAD} ${HEADERS}
