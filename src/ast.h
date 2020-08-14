#ifndef AST_H
#define AST_H

//===----------------------------------------------------------------------===//
// Abstract Syntax Tree (aka Parse Tree)
//===----------------------------------------------------------------------===//

#include <map>
#include <memory>
#include <vector>
#include <iostream>

class NodeAST {
public:
  virtual ~NodeAST() = default;
  virtual void visit() const = 0;
};

class ProgramAST : public NodeAST {
public:
    ProgramAST() {}
    std::vector<std::unique_ptr<NodeAST> > statements;
    void visit() const override {
        std::cout << "Visited Program" << std::endl; 
        for (const auto& statement : statements)
        {
            statement->visit();
        }
    }
};

/// ExprAST - Base class for all expression nodes.
class ExprAST : public NodeAST {
public:
    void visit() const override {
        std::cout << "Visited Expr" << std::endl; 
    }
};

/// NumberExprAST - Expression class for numeric literals like "1.0".
class NumberExprAST : public ExprAST {
  double Val;

public:
    void visit() const override {
        std::cout << "Visited NumberExpr" << std::endl; 
    }
  NumberExprAST(double Val) : Val(Val) {}
};

/// VariableExprAST - Expression class for referencing a variable, like "a".
class VariableExprAST : public ExprAST {
  std::string Name;

public:
  void visit() const override {
    std::cout << "Visited Variable" << std::endl; 
  }
  VariableExprAST(const std::string &Name) : Name(Name) {}
};

/// BinaryExprAST - Expression class for a binary operator.
class BinaryExprAST : public ExprAST {
  char Op;
  std::unique_ptr<ExprAST> LHS, RHS;

public:
  void visit() const override {
        std::cout << "Visited BinaryExpr" << std::endl;
        LHS->visit();
        RHS->visit();
    }
  BinaryExprAST(char Op, std::unique_ptr<ExprAST> LHS,
                std::unique_ptr<ExprAST> RHS)
      : Op(Op), LHS(std::move(LHS)), RHS(std::move(RHS)) {}
};

/// CallExprAST - Expression class for function calls.
class CallExprAST : public ExprAST {
  std::string Callee;
  std::vector<std::unique_ptr<ExprAST>> Args;

public:
  void visit() const override {
        std::cout << "Visited CallExpr" << std::endl;
        for(const auto& arg : Args)
        {
            arg->visit();
        }
  }
  CallExprAST(const std::string &Callee,
              std::vector<std::unique_ptr<ExprAST>> Args)
      : Callee(Callee), Args(std::move(Args)) {}
};

/// PrototypeAST - This class represents the "prototype" for a function,
/// which captures its name, and its argument names (thus implicitly the number
/// of arguments the function takes).
class PrototypeAST : public NodeAST {
  std::string Name;
  std::vector<std::string> Args;

public:
  void visit() const override {
        std::cout << "Visited Prototype" << std::endl;
  }
  PrototypeAST(const std::string &Name, std::vector<std::string> Args)
      : Name(Name), Args(std::move(Args)) {}

  const std::string &getName() const { return Name; }
};

/// FunctionAST - This class represents a function definition itself.
class FunctionAST : public NodeAST {
  std::unique_ptr<PrototypeAST> Proto;
  std::unique_ptr<ExprAST> Body;

public:
  void visit() const override {
    std::cout << "Visited Function" << std::endl;
    Proto->visit();
    Body->visit();
  }
  FunctionAST(std::unique_ptr<PrototypeAST> Proto,
              std::unique_ptr<ExprAST> Body)
      : Proto(std::move(Proto)), Body(std::move(Body)) {}
};

#endif
