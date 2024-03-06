all: main.cpp
	$(CXX) $(CXXFLAGS) -Og main.cpp -g -o displaytext.bin
clean:
	rm -f displaytext.bin
