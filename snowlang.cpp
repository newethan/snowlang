#include "snowlang.hpp"
#include "lexer.hpp"
#include "parser.hpp"

using namespace std;
using namespace snowlang;

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        cout << "Missing argument. Program terminated." << endl;
        exit(1);
    }
    const char *text1 =
        "mod 1bitreg {\n"
        "   input{\n"
        "       bit data;"
        "       bit set;"
        "   }"
        "   output{"
        "       bit value;"
        "   }"
        "   @ xor mem;"
        "   = mem mem;"
        "   @ or reset;"
        "   @ and switch_on;"
        "   @ and flip_if_on;"
        "   = set reset;"
        "   = reset flip_if_on;"
        "   = mem flip_if_on;"
        "   = flip_if_on mem;"
        "   = set switch_on;"
        "   = data switch_on;"
        "   = switch_on mem;"
        "   = mem value;"
        "}"
        "wire byte {"
        "    [8]bit data;"
        "}"
        "mod 8bitreg {"
        "    input{"
        "        byte data;"
        "        bit set;"
        "    }"
        "    output{"
        "        byte value;"
        "    }"
        "    @ [8]1bitreg registers;"
        "    loop $n in (1,8) {"
        "        = data[$n] registers[$n].data;"
        "        = set registers.[$n]set;"
        "        = registers[$n].value value[$n]"
        "    } "
        "}";
    const char *text2 = "2*2";

    lexer::Lexer lexer(argv[1], "test.sno");
    for (auto &token : lexer.tokens)
        cout << token.repr() << endl;

    // TODO: MAKE SURE THIS IS CREATING A COPY OF lexer.tokens
    parser::Parser parser(lexer.tokens);
    auto ast = parser.expression();
    parser::Parser::printAst(ast);
}