#include <iostream>
#include <fstream>

#include "lexer.hpp"
#include "token.hpp"
#include "parser.hpp"
#include "node.hpp"
#include "errorHandler.hpp"
#include "interpreter.hpp"

using namespace std;
using namespace snowlang;

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        cout << "Missing argument. Program terminated." << endl;
        exit(1);
    }
    string filename = argv[1];

    fstream file;
    file.open(filename, ios::in);
    if (!file)
    {
        cout << "File '" << filename << "' does not exist." << endl;
        exit(1);
    }
    stringstream buf;
    buf << file.rdbuf();
    string text = buf.str();

    try
    {
        lexer::Lexer l(text);
        auto tokens = l.lex();
        // for (auto &token : tokens)
        //     cout << token.repr() << endl;
        parser::Parser p(tokens);
        auto ast = p.parse();
        // printAst(ast);
        interpreter::Interpreter i(move(ast), filename, text);
        i.interpret();
    }
    catch (err::LexerParserException &e)
    {
        err::fatalErrorAbort(e.pos, filename, text, e.message);
    }
    catch (err::InterpreterException &e)
    {
        err::fatalErrorAbort(e.pos, e.filename, e.text, e.message);
    }
}