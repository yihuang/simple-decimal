OBJS=decimal.o
INC=-I./int128/src
MAC_LDFLAGS=-undefined dynamic_lookup

%.o : %.cpp
	$(CXX) -O3 -Wall $(INC) -std=c++11 -c $< -o $@

cdecimal: pybinding.cpp decimal.o
	$(CXX) -O3 -Wall -shared -o $@`python3-config --extension-suffix` pybinding.cpp -std=c++11 -fPIC `python3 -m pybind11 --includes` $(INC) $(OBJS) $(MAC_LDFLAGS)

test: cdecimal
	python3 test.py

clean:
	rm $(OBJS)
	rm test
