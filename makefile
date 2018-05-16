all:table genmtx philosopher
LIBFLAGS := -std=c++11 -larmadillo -Wall -pthread
IFLAGS := -Iinclude/
TABLESRC := src/Table.cpp
table: $(TABLESRC) src/tableMain.cpp include/Table.hpp
	g++ $(TABLESRC) $(IFLAGS) $(LIBFLAGS) src/tableMain.cpp -o table
genmtx:src/genmtx.cpp
	g++ $(LIBFLAGS) src/genmtx.cpp -o genmtx
philosopher:src/philosopher.cpp
	g++ $(LIBFLAGS) src/philosopher.cpp -o philosopher
.PHONY clean:
clean:
	rm -f table philosopher genmtx