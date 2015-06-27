CC = mpicxx
CXX = mpicxx
BOOST = /share/data/sharker/opt
MODELDIR = ./data/PersistenceTuples
CXXFLAGS = -DNDEBUG -O3 -std=c++11 -I../cluster-delegator/include -I./include -I$(MODELDIR) -I$(BOOST)/include
LDFLAGS = -L$(BOOST)/lib
LDFLAGS += -Wl,-rpath,"$(abspath $(BOOST))/lib"
LDLIBS = -lboost_serialization -lboost_thread -lboost_system -lboost_chrono
all: Subsample ComputeDistanceMatrix

SUBSAMPLE = source/Subsample.o
DISTANCE = source/ComputeDistanceMatrix.o

Subsample: $(SUBSAMPLE)
	$(CC) $(LDFLAGS) $(SUBSAMPLE) -o $@ $(LDLIBS)
	mv Subsample $(MODELDIR)

ComputeDistanceMatrix: $(DISTANCE)
	$(CC) $(LDFLAGS) $(DISTANCE) -o $@ $(LDLIBS)
	mv ComputeDistanceMatrix $(MODELDIR)
clean:
	rm -f ./source/*.o
