#include "snowlang.hpp"
#include "lexer.hpp"
using namespace std;

int main()
{
    const char *text =
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
    snowlang::lexer::Lexer lexer(text, "test.sno");
    while (lexer.hasMoreTokens())
    {
        cout << lexer.peek(0).repr() << endl;
        lexer.advance(1);
    }
}