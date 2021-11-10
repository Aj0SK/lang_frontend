1. Make sure that llvm-config is installed and in path. It is possible
that there is only a certain version of llvm-config, so you can do:

`sudo ln -s /usr/bin/llvm-config-10 /usr/local/bin/llvm-config`

2. After successful build you should be able to run:

`./build/example ./test/sample_code/a.txt`