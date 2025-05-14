# Compiler
CXX-MINGW = x86_64-w64-mingw32-g++.exe
CXX = g++.exe

CXXFLAGS = -std=c++17 -Wall -Wextra -static -static-libgcc -static-libstdc++
INCLUDE = -I./include/
LDFLAGS = -L./lib-mingw64/ -lftxui-component -lftxui-dom -lftxui-screen -lncurses

msys:
	$(CXX) $(CXXFLAGS) $(INCLUDE) -o ./out/main-$@.exe main.cpp $(LDFLAGS)

mingw:
	$(CXX-MINGW) $(CXXFLAGS) $(INCLUDE) -o ./out/main-$@.exe main.cpp $(LDFLAGS)

# Clean up build files
clean:
	rm -f ./out/*