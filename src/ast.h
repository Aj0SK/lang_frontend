#ifndef AST_H
#define AST_H

//===----------------------------------------------------------------------===//
// Abstract Syntax Tree (aka Parse Tree)
//===----------------------------------------------------------------------===//

#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"

#include <iostream>
#include <map>
#include <memory>
#include <vector>

using namespace llvm;

static LLVMContext TheContext;
static IRBuilder<> Builder(TheContext);
static std::unique_ptr<Module> TheModule;
static std::map<std::string, Value *> NamedValues;

class ExprAST;
class PrototypeAST;

Value *LogErrorV(const char *Str);
std::unique_ptr<ExprAST> LogError(const char *Str);
std::unique_ptr<PrototypeAST> LogErrorP(const char *Str);

void print_prefix(int depth);

class NodeAST
{
protected:
public:
  virtual ~NodeAST() = default;
  virtual void visit(int depth) const = 0;
  virtual Value *codegen() = 0;
};

class ProgramAST
{
public:
  ProgramAST() {}
  std::vector<std::unique_ptr<NodeAST>> statements;

  void visit(int depth) const
  {
    print_prefix(depth);
    std::cout << "Visited Program" << std::endl;
    for (const auto &statement : statements)
    {
      statement->visit(depth + 1);
    }
  };
  void codegen()
  {
    for (const auto &statement : statements)
    {
      if (auto *FnIR = statement->codegen())
      {
        fprintf(stderr, "Read function definition:");
        FnIR->print(errs());
        fprintf(stderr, "\n");
      }
    }
  }
};

/// ExprAST - Base class for all expression nodes.
class ExprAST : public NodeAST
{
public:
  void visit(int depth) const override
  {
    print_prefix(depth);
    std::cout << "Visited Expr" << std::endl;
  }
  Value *codegen() override = 0;
};

/// NumberExprAST - Expression class for numeric literals like "1.0".
class NumberExprAST : public ExprAST
{
  double Val;

public:
  void visit(int depth) const override
  {
    print_prefix(depth);
    std::cout << "Visited NumberExpr" << std::endl;
  }
  NumberExprAST(double Val) : Val(Val) {}
  Value *codegen() override;
};

/// VariableExprAST - Expression class for referencing a variable, like "a".
class VariableExprAST : public ExprAST
{
  std::string Name;

public:
  void visit(int depth) const override
  {
    print_prefix(depth);
    std::cout << "Visited Variable" << std::endl;
  }
  VariableExprAST(const std::string &Name) : Name(Name) {}
  Value *codegen() override;
};

/// BinaryExprAST - Expression class for a binary operator.
class BinaryExprAST : public ExprAST
{
  char Op;
  std::unique_ptr<ExprAST> LHS, RHS;

public:
  void visit(int depth) const override
  {
    print_prefix(depth);
    std::cout << "Visited BinaryExpr" << std::endl;
    LHS->visit(depth + 1);

    print_prefix(depth + 1);
    std::cout << Op << std::endl;

    RHS->visit(depth + 1);
  }
  BinaryExprAST(char Op, std::unique_ptr<ExprAST> LHS,
                std::unique_ptr<ExprAST> RHS)
      : Op(Op), LHS(std::move(LHS)), RHS(std::move(RHS))
  {
  }
  Value *codegen() override;
};

/// CallExprAST - Expression class for function calls.
class CallExprAST : public ExprAST
{
  std::string Callee;
  std::vector<std::unique_ptr<ExprAST>> Args;

public:
  void visit(int depth) const override
  {
    print_prefix(depth);
    std::cout << "Visited CallExpr" << std::endl;
    for (const auto &arg : Args)
    {
      arg->visit(depth + 1);
    }
  }
  CallExprAST(const std::string &Callee,
              std::vector<std::unique_ptr<ExprAST>> Args)
      : Callee(Callee), Args(std::move(Args))
  {
  }
  Value *codegen() override;
};

/// PrototypeAST - This class represents the "prototype" for a function,
/// which captures its name, and its argument names (thus implicitly the number
/// of arguments the function takes).
class PrototypeAST : public NodeAST
{
  std::string Name;
  std::vector<std::string> Args;

public:
  void visit(int depth) const override
  {
    print_prefix(depth);
    std::cout << "Visited Prototype" << std::endl;
  }
  PrototypeAST(const std::string &Name, std::vector<std::string> Args)
      : Name(Name), Args(std::move(Args))
  {
  }

  const std::string &getName() const { return Name; }
  Function *codegen() override;
};

/// FunctionAST - This class represents a function definition itself.
class FunctionAST : public NodeAST
{
  std::unique_ptr<PrototypeAST> Proto;
  std::unique_ptr<ExprAST> Body;

public:
  void visit(int depth) const override
  {
    print_prefix(depth);
    std::cout << "Visited Function" << std::endl;
    Proto->visit(depth + 1);
    Body->visit(depth + 1);
  }
  FunctionAST(std::unique_ptr<PrototypeAST> Proto,
              std::unique_ptr<ExprAST> Body)
      : Proto(std::move(Proto)), Body(std::move(Body))
  {
  }
  Function *codegen() override;
};

#endif
