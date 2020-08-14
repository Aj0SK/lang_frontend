#include <iostream>
#include "driver.hh"

int main (int argc, char *argv[])
{
  int res = 0;
  driver drv;
  std::vector<std::string> inputs { "def f(x) { x+2.0*3.0 }"};
  
  for(const auto& input : inputs)
  {
    std::cout << "Input is " << input << std::endl;
    drv.parse_string (input);
    auto tree = std::move(drv.result);
    tree->visit();
    std::cout << "Result is " << tree.get() << std::endl;
  }
  
  return res;
}
