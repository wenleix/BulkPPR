CC = g++
LD = g++

CFLAGS=-O2 -std=c++0x
LFLAGS=-O2

.PHONY : clean 

all: ConvertGraph Convert2Metis BinGraph2Metis
	
# 	Executables
ConvertGraph: ConvertGraph.cpp
	$(CC) $(CFLAGS) ConvertGraph.cpp -o ConvertGraph

Convert2Metis: Convert2Metis.cpp
	$(CC) $(CFLAGS) Convert2Metis.cpp -o Convert2Metis

BinGraph2Metis: BinGraph2Metis.cpp
	$(CC) $(CFLAGS) BinGraph2Metis.cpp -o BinGraph2Metis

clean:
	rm ConvertGraph Convert2Metis BinGraph2Metis

