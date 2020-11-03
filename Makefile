CXXFLAGS = -O3 -std=c++17 -Wall -Wextra -DNDEBUG -march=native
LDFLAGS = -flto=thin -lpthread
OBJS = Main.o AStar.o

all: solver

solver: $(OBJS)
	$(CXX) $(LDFLAGS) -L/tmp2/b07902024/abseil-cpp/build/install/lib $^ -o $@ -labsl_hash   -labsl_raw_hash_set -labsl_city

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -I/tmp2/b07902024/abseil-cpp/build/install/include -c $< -o $@

clean:
	$(RM) solver $(OBJS)
