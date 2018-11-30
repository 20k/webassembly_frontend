#include <iostream>
#include <string>
#include <string_view>
#include <fstream>
#include <direct.h>

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

std::string read_file_bin(const std::string& file)
{
    std::ifstream t(file, std::ios::binary);
    std::string str((std::istreambuf_iterator<char>(t)),
                     std::istreambuf_iterator<char>());

    if(!t.good())
        throw std::runtime_error("Could not open file " + file);

    return str;
}

void write_all_bin(const std::string& fname, const std::string& str)
{
    std::ofstream out(fname, std::ios::binary);
    out << str;
}

bool file_exists(const std::string& name)
{
    std::ifstream f(name.c_str());
    return f.good();
}

void copy_file(const std::string& f1, const std::string& f2)
{
    write_all_bin(f2, read_file_bin(f1));
}

int main(int argc, char* argv[])
{
    if(argc <= 1)
    {
        printf("Usage: front.exe <filename>");
        return 1;
    }

    std::string file = argv[1];

    if(!file_exists(file))
    {
        printf("File does not exist");
        return 1;
    }

    auto it = file.find_last_of('.');

    if(it == std::string::npos)
    {
        printf("No file extension?");
        return 1;
    }

    std::string root_name(file.begin(), file.begin() + it);

    std::string path = "./compile/";

    mkdir(path.c_str());

    copy_file(file, path + file);

    std::string new_file = path + file;

    if(ends_with(new_file, ".c") || ends_with(new_file, ".cpp"))
    {
        auto last = new_file.find_last_of('.');

        if(last == std::string::npos)
            return 1;

        std::string fname(new_file.begin(), new_file.begin() + last);

        std::string clang = "c:/cllvm2/bin/clang -emit-llvm --target=wasm32 -O3 " + new_file + " -c -o " + fname + ".bc -IC:/cllvm2/include -IC:/cllvm2/include/c++/v1";
        std::string llc1 = "c:/cllvm2/bin/llc -march=wasm32 -filetype=obj " + fname + ".bc -o " + fname + ".o";
        std::string llc2 = "c:/cllvm2/bin/llc -march=wasm32 " + fname + ".bc -o " + fname + ".s";
        std::string lld = "c:/cllvm2/bin/lld -flavor wasm --export-all --allow-undefined " + fname + ".o -o " + fname + ".wasm -LC:/cllvm2/lib -lc --no-entry";

        system(clang.c_str());
        system(llc1.c_str());
        system(llc2.c_str());
        system(lld.c_str());

        copy_file(fname + ".wasm", root_name + ".wasm");

        return 0;
    }

    else if(ends_with(new_file, ".js") || ends_with(new_file, ".ts"))
    {
        auto last = new_file.find_last_of('.');

        if(last == std::string::npos)
            return 1;

        std::string fname(new_file.begin(), new_file.begin() + last);

        if(ends_with(new_file, ".js"))
        {
            write_all_bin(fname + ".ts", read_file_bin(new_file));
        }

        std::string current_filename = fname + ".ts";

        std::string to_exec = "node compile_js.js " + current_filename;

        system(to_exec.c_str());

        copy_file(fname + ".ts.wasm", root_name + ".wasm");
    }
    else
    {
        printf("unknown filetype");
        return 1;
    }

    return 0;
}
