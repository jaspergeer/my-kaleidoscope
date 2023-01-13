CPP=clang++
CPPFLAGS=`llvm-config --cxxflags`
INCLUDES = parser.h lexer.h

%.o: %.cpp
	$(CPP) -c $^ $(CPPFLAGS)

clean:
	rm *.o *.h.gc