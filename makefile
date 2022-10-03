snowlang: main.o lexer.o parser.o errorHandler.o logic.o interpreter.o symbol.o
	g++ main.o lexer.o parser.o errorHandler.o logic.o interpreter.o symbol.o -o snowlang -Wall -pedantic -g

main.o: main.cpp lexer.hpp parser.hpp node.hpp errorHandler.hpp
	g++ -c main.cpp -Wall -pedantic -g

lexer.o: lexer.cpp lexer.hpp errorHandler.hpp token.hpp
	g++ -c lexer.cpp -Wall -pedantic -g

parser.o: parser.cpp parser.hpp node.hpp errorHandler.hpp
	g++ -c parser.cpp -Wall -pedantic -g

errorHandler.o: errorHandler.cpp errorHandler.hpp token.hpp
	g++ -c errorHandler.cpp -Wall -pedantic -g

logic.o: logic.cpp logic.hpp
	g++ -c logic.cpp -Wall -pedantic -g

interpreter.o: interpreter.cpp interpreter.hpp node.hpp logic.hpp errorHandler.hpp
	g++ -c interpreter.cpp -Wall -pedantic -g

symbol.o: symbol.cpp symbol.hpp node.hpp errorHandler.hpp
	g++ -c symbol.cpp -Wall -pedantic -g

clean:
	rm *.o snowlang