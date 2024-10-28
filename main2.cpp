#include <iostream>
#include <filesystem>
#include <map>

using namespace std;
namespace fs = filesystem;

int main()
{
    map<string, int> result;
    for (const auto file : fs::directory_iterator("./"))
    {
        if (fs::is_regular_file(file))
            result["regular file"]++;
        else if (fs::is_directory(file))
            result["directory"]++;
        else if (fs::is_block_file(file))
            result["block device"]++;
        else if (fs::is_character_file(file))
            result["character device"]++;
        else if (fs::is_fifo(file))
            result["named pipe"]++;
        else if (fs::is_socket(file))
            result["socket"]++;
        else if (fs::is_symlink(file))
            result["symbolic links"]++;
        else
            result["unknown"]++;
    }

    for (auto [key, value] : result)
    {
        cout << key << ": " << value << endl;
    }
}