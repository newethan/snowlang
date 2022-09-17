#include "snowlang.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "program.hpp"

using namespace std;
using namespace snowlang;

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        cout << "Missing argument. Program terminated." << endl;
        exit(1);
    }
    string text1 =
        "mod 1bitreg {\n"
        "   input{\n"
        "       bit data;\n"
        "       bit set;\n"
        "   }\n"
        "   output{\n"
        "       bit value;\n"
        "   }\n"
        "   @ xor mem;\n"
        "   = mem mem;\n"
        "   @ or reset;\n"
        "   @ and switch_on;\n"
        "   @ and flip_if_on;\n"
        "   = set reset;\n"
        "   = reset flip_if_on;\n"
        "   = mem flip_if_on;\n"
        "   = flip_if_on mem;\n"
        "   = set switch_on;\n"
        "   = data switch_on;\n"
        "   = switch_on mem;\n"
        "   = mem value;\n"
        "}\n"
        "wire byte {\n"
        "    [8]bit data;\n"
        "}\n"
        "mod 8bitreg {\n"
        "    input{\n"
        "        byte data;\n"
        "        bit set;\n"
        "    }\n"
        "    output{\n"
        "        byte value;\n"
        "    }\n"
        "    @ [8]1bitreg registers;\n"
        "    loop $n in (1,8) {\n"
        "        = data[$n] registers[$n].data;\n"
        "        = set registers.[$n]set;\n"
        "        = registers[$n].value value[$n]\n"
        "    } \n"
        "}";
    string text2 = "2*2";

    auto tokens = lexer::lex(argv[1]);
    for (auto &token : tokens)
        cout << token.repr() << endl;

    auto ast = parser::parse(tokens);
    parser::printAst(ast);

    Program prog(text1, "test.sno");
    prog.fatalErrorAbort(31, 33, "Unexpected Token");
}