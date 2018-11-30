#include <iostream>
#include <string>
#include <string_view>

/*clang -emit-llvm --target=wasm32 -O3 test_1.c -c -o test_1.bc -IC:/cllvm2/include
llc -march=wasm32 -filetype=obj test_1.bc -o test_1.o
llc -march=wasm32 test_1.bc -o test_1.s
lld -flavor wasm --export-all --allow-undefined test_1.o -o test_1.wasm -LC:/cllvm2/lib -lc*/

///building with llvm
bool ends_with(std::string const & value, std::string const & ending)
{
    if (ending.size() > value.size())
        return false;

    return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

int main(int argc, char* argv[])
{
    if(argc <= 1)
    {
        printf("Usage: front.exe <filename>");
        return 1;
    }

    std::string file = argv[1];

    if(ends_with(file, ".c") || ends_with(file, ".cpp"))
    {
        auto last = file.find_last_of('.');

        if(last == std::string::npos)
            return 1;

        std::string fname(file.begin(), file.begin() + last);

        std::string clang = "clang -emit-llvm --target=wasm32 -O3 " + file + " -c -o " + fname + ".bc -IC:/cllvm2/include";
        std::string llc1 = "llc -march=wasm32 -filetype=obj " + fname + ".bc -o " + fname + ".o";
        std::string llc2 = "llc -march=wasm32 " + fname = ".bc -o " + fname + ".s";
        std::string lld = "lld -flavor wasm --export-all --allow-undefined " + fname + ".o -o " + fname + ".wasm -LC:/cllvm/lib -lc";

        system(clang.c_str());
        system(llc1.c_str());
        system(llc2.c_str());
        system(lld.c_str());

        return 0;
    }

    return 0;
}
