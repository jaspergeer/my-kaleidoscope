CC = clang++ -stdlib=libc++ -std=c++14
CFLAGS = -g -O3 -I llvm/include -I llvm/build/include -I ./
LLVMFLAGS = `llvm-config --cxxflags --ldflags --system-libs --libs all`

%.o: %.cpp
	$(CC) -c $(CPPFLAGS) $< -o $@

main: driver.o parser.o error.o ast.o codegen.o
	${CC} ${CFLAGS} ${LLVMFLAGS} $^ -o $@

clean:
	rm -r *.o main *.dSYM
