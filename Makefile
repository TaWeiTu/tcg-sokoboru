CXXFLAGS = -O3 -std=c++17 -Wall -Wextra -DNDEBUG -march=native
LDFLAGS = -flto=thin
OBJS = Main.o

all: solver

solver: $(OBJS)
	$(CXX) $(LDFLAGS) $^ -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	$(RM) solver $(OBJS)
