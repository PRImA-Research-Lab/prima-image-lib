CPP=g++
CFLAGS=-fPIC -g -c -Wall -pedantic
INCLUDES=-I../dla/ -I../extra/ -I../image/ -I../xml/
INSTALLLIB=install -m 644 -t /usr/lib/
HEADINSTALLDIR=/usr/include/image/
INSTALLHEAD=install -m 644 -t ${HEADINSTALLDIR}
HEADERS=BiLevelImage.h CompassEdgeDetector.h ConnectedComponents.h EdgeDetector.h GreyScaleImage.h HiColorImage.h Histogram.h Image.h LinuxDefs.h LoColorImage.h RegionMap.h Run.h

all: libimage.so

libimage.so: BiLevelImage.o CompassEdgeDetector.o ConnectedComponent.o ConnectedComponents.o EdgeDetector.o GreyScaleImage.o HiColorImage.o Histogram.o Image.o LoColorImage.o RegionMap.o Run.o
	${CPP} -shared -Wl,-soname,libimage.so -o libimage.so BiLevelImage.o CompassEdgeDetector.o ConnectedComponent.o ConnectedComponents.o EdgeDetector.o GreyScaleImage.o HiColorImage.o Histogram.o Image.o LoColorImage.o RegionMap.o Run.o

BiLevelImage.o: BiLevelImage.h BiLevelImage.cpp
	${CPP} ${CFLAGS} ${INCLUDES} BiLevelImage.cpp

CompassEdgeDetector.o: CompassEdgeDetector.h CompassEdgeDetector.cpp
	${CPP} ${CFLAGS} ${INCLUDES} CompassEdgeDetector.cpp

ConnectedComponent.o: ConnectedComponent.h ConnectedComponent.cpp
	${CPP} ${CFLAGS} ${INCLUDES} ConnectedComponent.cpp

ConnectedComponents.o: ConnectedComponents.h ConnectedComponents.cpp
	${CPP} ${CFLAGS} ${INCLUDES} ConnectedComponents.cpp

EdgeDetector.o: EdgeDetector.h EdgeDetector.cpp
	${CPP} ${CFLAGS} ${INCLUDES} EdgeDetector.cpp

GreyScaleImage.o: GreyScaleImage.h GreyScaleImage.cpp
	${CPP} ${CFLAGS} ${INCLUDES} GreyScaleImage.cpp

HiColorImage.o: HiColorImage.h HiColorImage.cpp
	${CPP} ${CFLAGS} ${INCLUDES} HiColorImage.cpp

Histogram.o: Histogram.h Histogram.cpp
	${CPP} ${CFLAGS} ${INCLUDES} Histogram.cpp

Image.o: Image.h Image.cpp
	${CPP} ${CFLAGS} ${INCLUDES} Image.cpp

LoColorImage.o: LoColorImage.h LoColorImage.cpp
	${CPP} ${CFLAGS} ${INCLUDES} LoColorImage.cpp

RegionMap.o: RegionMap.h RegionMap.cpp
	${CPP} ${CFLAGS} ${INCLUDES} RegionMap.cpp

Run.o: Run.h Run.cpp
	${CPP} ${CFLAGS} ${INCLUDES} Run.cpp

clean:
	rm -f BiLevelImage.o CompassEdgeDetector.o ConnectedComponent.o ConnectedComponents.o EdgeDetector.o GreyScaleImage.o HiColorImage.o Histogram.o Image.o LoColorImage.o RegionMap.o Run.o libimage.so

install:
	${INSTALLLIB} libimage.so
	mkdir -p ${HEADINSTALLDIR}
	${INSTALLHEAD} ${HEADERS}
