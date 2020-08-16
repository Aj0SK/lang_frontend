#include "ast.h"

using namespace llvm;

void print_prefix(int depth)
{
  for (int i = 0; i < depth; ++i)
    std::cout << '\t';
};

Value *LogErrorV(const char *Str)
{
  LogError(Str);
  return nullptr;
}

std::unique_ptr<ExprAST> LogError(const char *Str)
{
  fprintf(stderr, "Error: %s\n", Str);
  return nullptr;
}

std::unique_ptr<PrototypeAST> LogErrorP(const char *Str)
{
  LogError(Str);
  return nullptr;
}

Value *NumberExprAST::codegen(CodeContext& cc)
{
  return ConstantFP::get(cc.TheContext, APFloat(Val));
}

Value *VariableExprAST::codegen(CodeContext& cc)
{
  // Look this variable up in the function.
  Value *V = cc.NamedValues[Name];
  if (!V)
    return LogErrorV("Unknown variable name");
  return V;
}

Value *BinaryExprAST::codegen(CodeContext& cc)
{
  Value *L = LHS->codegen(cc);
  Value *R = RHS->codegen(cc);
  if (!L || !R)
    return nullptr;

  switch (Op)
  {
  case '+':
    return cc.Builder.CreateFAdd(L, R, "addtmp");
  case '-':
    return cc.Builder.CreateFSub(L, R, "subtmp");
  case '*':
    return cc.Builder.CreateFMul(L, R, "multmp");
  case '<':
    L = cc.Builder.CreateFCmpULT(L, R, "cmptmp");
    // Convert bool 0/1 to double 0.0 or 1.0
    return cc.Builder.CreateUIToFP(L, Type::getDoubleTy(cc.TheContext), "booltmp");
  default:
    return LogErrorV("invalid binary operator");
  }
}

Value *CallExprAST::codegen(CodeContext& cc)
{
  // Look up the name in the global module table.
  Function *CalleeF = cc.TheModule->getFunction(Callee);
  if (!CalleeF)
    return LogErrorV("Unknown function referenced");

  // If argument mismatch error.
  if (CalleeF->arg_size() != Args.size())
    return LogErrorV("Incorrect # arguments passed");

  std::vector<Value *> ArgsV;
  for (unsigned i = 0, e = Args.size(); i != e; ++i)
  {
    ArgsV.push_back(Args[i]->codegen(cc));
    if (!ArgsV.back())
      return nullptr;
  }

  return cc.Builder.CreateCall(CalleeF, ArgsV, "calltmp");
}

Function *PrototypeAST::codegen(CodeContext& cc)
{
  // Make the function type:  double(double,double) etc.
  std::vector<Type *> Doubles(Args.size(), Type::getDoubleTy(cc.TheContext));
  FunctionType *FT =
      FunctionType::get(Type::getDoubleTy(cc.TheContext), Doubles, false);

  Function *F =
      Function::Create(FT, Function::ExternalLinkage, Name, cc.TheModule.get());

  // Set names for all arguments.
  unsigned Idx = 0;
  for (auto &Arg : F->args())
    Arg.setName(Args[Idx++]);

  return F;
}

Function *FunctionAST::codegen(CodeContext& cc)
{
  // First, check for an existing function from a previous 'extern' declaration.
  Function *TheFunction = cc.TheModule->getFunction(Proto->getName());

  if (!TheFunction)
    TheFunction = Proto->codegen(cc);

  if (!TheFunction)
    return nullptr;

  // Create a new basic block to start insertion into.
  BasicBlock *BB = BasicBlock::Create(cc.TheContext, "entry", TheFunction);
  cc.Builder.SetInsertPoint(BB);

  // Record the function arguments in the NamedValues map.
  cc.NamedValues.clear();
  for (auto &Arg : TheFunction->args())
    cc.NamedValues[std::string(Arg.getName())] = &Arg;

  if (Value *RetVal = Body->codegen(cc))
  {
    // Finish off the function.
    cc.Builder.CreateRet(RetVal);

    // Validate the generated code, checking for consistency.
    verifyFunction(*TheFunction);

    return TheFunction;
  }

  // Error reading body, remove function.
  TheFunction->eraseFromParent();
  return nullptr;
}
