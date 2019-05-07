CXX = g++

# Warnings frequently signal eventual errors:
CXXFLAGS= -g -W -Wall -std=c++11 -pthread

OBJS = \
	server.o
EXEC = server

%.o: %.cpp %.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(EXEC): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS)

clean:
	rm -rf $(OBJS)
	rm -rf $(EXEC)
