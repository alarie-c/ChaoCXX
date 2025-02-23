#include <iostream>
#include <string_view>
#include "ast.hpp"

Node::Node(Type t, size_t y, size_t x0, size_t x1) : type(t), y(y), x0(x0), x1(x1) {
  switch (t) {
  case NODE_EXPR_BINARY: {
    this->binary.lhs = nullptr;
    this->binary.rhs = nullptr;
    this->binary.op = nullptr;
  }
  }
}

Node::~Node() {
  switch (this->type) {
  case NODE_EXPR_BINARY: {
    delete this->binary.lhs;
    delete this->binary.rhs;
    delete this->binary.op;
  }
  }
}

std::ostream& operator<<(std::ostream& os, const Node& node) {
    switch (node.type) {
    case Node::NODE_EXPR_STRING:
        os << "String: " << node.string.value;
        break;
    case Node::NODE_EXPR_SYMBOL:
        os << "Symbol: " << node.symbol.name;
        break;
    case Node::NODE_EXPR_NUMBER:
        os << "Number: " << node.number.value;
        break;
    case Node::NODE_EXPR_BINARY:
        os << "Binary {\n";
        os << "  lhs: " << *node.binary.lhs << "\n";
        os << "  rhs: " << *node.binary.rhs << "\n";
        os << "  op: " << node.binary.op << "\n";
        os << "}";
        break;
    }
    return os;
}