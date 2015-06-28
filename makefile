CC = mpicxx
CXX = mpicxx
BOOST = /share/data/sharker/opt
CXXFLAGS = -DNDEBUG -O3 -std=c++14 -I../cluster-delegator/include -I./include -I$(BOOST)/include
LDFLAGS = -L$(BOOST)/lib
LDFLAGS += -Wl,-rpath,"$(abspath $(BOOST))/lib"
LDLIBS = -lboost_serialization -lboost_thread -lboost_system -lboost_chrono
all: Subsample ComputeDistanceMatrix

SUBSAMPLE = source/Subsample.o
DISTANCE = source/ComputeDistanceMatrix.o

Subsample: $(SUBSAMPLE)
	$(CC) $(LDFLAGS) $(SUBSAMPLE) -o $@ $(LDLIBS)
	mv Subsample ./bin
ComputeDistanceMatrix: $(DISTANCE)
	$(CC) $(LDFLAGS) $(DISTANCE) -o $@ $(LDLIBS)
	mv ComputeDistanceMatrix ./bin
clean:
	rm -f ./source/*.o
	rm -f ./bin/Subsample
	rm -f ./bin/ComputeDistanceMatrix
