snowlang: src/main.o src/lexer.o src/parser.o src/errorHandler.o src/logic.o \
src/interpreter.o src/symbol.o
	g++ src/main.o src/lexer.o src/parser.o src/errorHandler.o src/logic.o \
src/interpreter.o src/symbol.o -o snowlang -Wall -pedantic -g

src/main.o: src/main.cpp src/lexer.hpp src/parser.hpp src/node.hpp \
src/errorHandler.hpp
	g++ -c src/main.cpp -Wall -pedantic -g -o src/main.o

src/lexer.o: src/lexer.cpp src/lexer.hpp src/errorHandler.hpp src/token.hpp
	g++ -c src/lexer.cpp -Wall -pedantic -g -o src/lexer.o

src/parser.o: src/parser.cpp src/parser.hpp src/node.hpp src/errorHandler.hpp
	g++ -c src/parser.cpp -Wall -pedantic -g -o src/parser.o

src/errorHandler.o: src/errorHandler.cpp src/errorHandler.hpp src/token.hpp
	g++ -c src/errorHandler.cpp -Wall -pedantic -g -o src/errorHandler.o

src/logic.o: src/logic.cpp src/logic.hpp
	g++ -c src/logic.cpp -Wall -pedantic -g -o src/logic.o

src/interpreter.o: src/interpreter.cpp src/interpreter.hpp src/node.hpp \
src/logic.hpp src/errorHandler.hpp
	g++ -c src/interpreter.cpp -Wall -pedantic -g -o src/interpreter.o

src/symbol.o: src/symbol.cpp src/symbol.hpp src/node.hpp src/errorHandler.hpp
	g++ -c src/symbol.cpp -Wall -pedantic -g -o src/symbol.o

clean:
	rm src/*.o snowlang