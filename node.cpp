#include "snowlang.hpp"
#include "node.hpp"

using namespace std;

namespace snowlang
{
    string Node::reprNodeType(enum NodeType nodeType)
    {
        return (const char *[]){
            "NT_BINOP",
            "NT_UNOP",
            "NT_LEAF",
            "NT_RANGE",
            "NT_TYPE",
            "NT_ITEM"}[nodeType];
    }
}