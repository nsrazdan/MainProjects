CXX = g++
CXXFLAGS = -Wall -Werror -std=c++11

INDEX_MIN_PQ_TESTER_OBJECTS = index_min_pq_tester.o
SHORTEST_PATH_OBJECTS = shortest_path.o

all: index_min_pq_tester shortest_path

index_min_pq_tester: $(INDEX_MIN_PQ_TESTER_OBJECTS)
	$(CXX) $(CXXFLAGS) -o index_min_pq_tester $(INDEX_MIN_PQ_TESTER_OBJECTS)

shortest_path: $(SHORTEST_PATH_OBJECTS)
	$(CXX) $(CXXFLAGS) -o shortest_path $(SHORTEST_PATH_OBJECTS)

$(INDEX_MIN_PQ_TESTER_OBJECTS): index_min_pq.h
$(SHORTEST_PATH_OBJECTS): index_min_pq.h

clean:
	rm *.o
	rm index_min_pq_tester
	rm shortest_path

lint:
	/home/cs36c/public/cpplint/cpplint *.cc
	/home/cs36c/public/cpplint/cpplint *.h
