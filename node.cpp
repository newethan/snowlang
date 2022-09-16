#include "snowlang.hpp"
#include "node.hpp"

using namespace std;

namespace snowlang
{
    Node::Node(
        enum NodeType t_type,
        variant<NumberValue, BinOpValue> t_value)
        : type(t_type), value(t_value) {}

    string Node::reprNodeType(enum NodeType nodeType)
    {
        return (const char *[]){
            "NT_BINOP",
            "NT_NUMBER"}[nodeType];
    }
}