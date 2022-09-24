#include <iostream>
#include <fstream>

#include "lexer.hpp"
#include "token.hpp"
#include "parser.hpp"
#include "node.hpp"
#include "errorHandler.hpp"

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

    vector<Token> tokens;
    try
    {
        tokens = lexer::lex(text);
    }
    catch (err::SnowlangException &e)
    {
        err::fatalErrorAbort(
            text,
            filename,
            e.posStart,
            e.posEnd,
            e.message);
    }
    // for (auto &token : tokens)
    //     cout << token.repr() << endl;
    unique_ptr<Node> ast;
    try
    {
        parser::Parser p(tokens);
        ast = p.parse();
    }
    catch (err::SnowlangException &e)
    {
        err::fatalErrorAbort(
            text,
            filename,
            e.posStart,
            e.posEnd,
            e.message);
    }
    printAst(ast);
}