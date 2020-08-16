#include "driver.hh"
#include <iostream>

int main(int argc, char *argv[])
{
  if (argc != 2)
  {
    std::cout << "Bad number of input parameters." << std::endl;
    std::cout << "Param is: input_file_path" << std::endl;
    return 1;
  }

  std::string path(argv[1]);
  driver drv;

  CodeContext cc;
  
  std::cout << "Input is\n" << path << std::endl;

  drv.parse_file(path);
  auto tree = std::move(drv.result);
  tree->visit(0);
  tree->codegen(cc);

  std::cout << "Result is " << tree.get() << std::endl;

  cc.TheModule->print(errs(), nullptr);

  return 0;
}
