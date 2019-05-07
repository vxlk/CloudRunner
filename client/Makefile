CXX = g++

# Warnings frequently signal eventual errors:
CXXFLAGS= -g -W -Wall -std=c++11

OBJS = \
	client.o
EXEC = client

%.o: %.cpp %.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(EXEC): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS)

clean:
	rm -rf $(OBJS)
	rm -rf $(EXEC)
