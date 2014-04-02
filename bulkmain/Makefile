CC = g++
LD = g++

CFLAGS=-O2 -std=c++0x
LFLAGS=-O2

.PHONY : clean 

all: bin/BlockPPR bin/BlockExpansion bin/BoundaryVtxCount

# Objective Files
obj/SimpleGraph.o: include/SimpleGraph.h src/SimpleGraph.cpp
	$(CC) -c $(CFLAGS) src/SimpleGraph.cpp -o obj/SimpleGraph.o -I./include

obj/PPRCommon.o: include/PPRCommon.h src/PPRCommon.cpp include/SimpleGraph.h
	$(CC) -c $(CFLAGS) src/PPRCommon.cpp -o obj/PPRCommon.o -I./include


# Executables
bin/BlockPPR: src/BlockPPR.cpp include/SimpleGraph.h include/PPRCommon.h \
	obj/SimpleGraph.o obj/PPRCommon.o
	$(CC) $(CFLAGS) src/BlockPPR.cpp obj/SimpleGraph.o obj/PPRCommon.o -I./include \
	-o bin/BlockPPR
	
bin/BlockExpansion: src/BlockExpansion.cpp include/SimpleGraph.h obj/SimpleGraph.o
	$(CC) $(CFLAGS) src/BlockExpansion.cpp obj/SimpleGraph.o -I./include \
	-o bin/BlockExpansion
 
bin/BoundaryVtxCount: src/BoundaryVtxCount.cpp include/SimpleGraph.h obj/SimpleGraph.o 
	$(CC) $(CFLAGS) src/BoundaryVtxCount.cpp obj/SimpleGraph.o -I./include \
	-o bin/BoundaryVtxCount


clean:
	rm bin/* obj/*

