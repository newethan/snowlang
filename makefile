snowlang: main.o lexer.o token.o parser.o node.o errorHandler.o
	g++ main.o lexer.o token.o parser.o node.o errorHandler.o -o snowlang -Wall -pedantic -g

main.o: main.cpp lexer.hpp parser.hpp node.hpp errorHandler.hpp
	g++ -c main.cpp -Wall -pedantic -g

lexer.o: lexer.cpp lexer.hpp errorHandler.hpp token.hpp
	g++ -c lexer.cpp -Wall -pedantic -g

token.o: token.cpp token.hpp
	g++ -c token.cpp -Wall -pedantic -g

parser.o: parser.cpp parser.hpp node.hpp errorHandler.hpp
	g++ -c parser.cpp -Wall -pedantic -g

node.o: node.cpp node.hpp token.hpp
	g++ -c node.cpp -Wall -pedantic -g

errorHandler.o: errorHandler.cpp errorHandler.hpp token.hpp
	g++ -c errorHandler.cpp -Wall -pedantic -g

clean:
	rm *.o snowlang