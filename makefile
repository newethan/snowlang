snowlang: main.o lexer.o token.o parser.o node.o errorHandler.o
	g++ main.o lexer.o token.o parser.o node.o errorHandler.o -o snowlang -Wall -pedantic -g

main.o: main.cpp lexer.hpp parser.hpp node.hpp errorHandler.hpp
	g++ -c main.cpp

lexer.o: lexer.cpp lexer.hpp errorHandler.hpp token.hpp
	g++ -c lexer.cpp

token.o: token.cpp token.hpp
	g++ -c token.cpp

parser.o: parser.cpp parser.hpp node.hpp errorHandler.hpp
	g++ -c parser.cpp

node.o: node.cpp node.hpp token.hpp
	g++ -c node.cpp

errorHandler.o: errorHandler.cpp errorHandler.hpp token.hpp
	g++ -c errorHandler.cpp

clean:
	rm *.o snowlang