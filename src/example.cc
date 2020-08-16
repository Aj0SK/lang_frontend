#include "driver.hh"
#include <iostream>

int main()
{
  int res = 0;
  driver drv;
  std::vector<std::string> inputs{"5.2\n5.3\nextern sin(x)\n"};

  for (const auto &input : inputs)
  {
    std::cout << "Input is " << input << std::endl;

    TheModule = std::make_unique<Module>("my cool jit", TheContext);

    drv.parse_string(input);
    auto tree = std::move(drv.result);
    tree->visit(0);
    tree->codegen();

    std::cout << "Result is " << tree.get() << std::endl;

    // TheModule->print(errs(), nullptr);
  }

  return res;
}
