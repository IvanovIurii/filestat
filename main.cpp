#include <unistd.h>
#include <iostream>
#include <string>
#include <sstream>
#include <map>

using namespace std;

vector<string> getOutputLines(int pipeFileDescriptor);
map<char, string> getAvailableFiletypesMap(void);
map<string, int> getFileTypesCountMap(vector<string> lines, map<char, string> availableFileTypes);

int main()
{
    // pfd[0] - read
    // pfd[1] - write
    int pfd[2];
    pipe(pfd);

    pid_t pid = fork();

    if (pid == 0)
    {
        close(pfd[0]); // because we do not read from pipe
        dup2(pfd[1], STDOUT_FILENO);

        int statusCode = execl("/bin/ls", "/bin/ls", "-la", ".", NULL);
        close(pfd[1]);

        if (statusCode == -1)
        {
            cout << "Can not execute 'ls' command" << endl;
            return 1;
        }

        return 0;
    }
    else if (pid > 0)
    {
        close(pfd[1]); // because we only read from pipe, but do not write into
        wait(NULL);

        vector<string> lines = getOutputLines(pfd[0]);
        close(pfd[0]);

        map<char, string> fileTypesMap = getAvailableFiletypesMap();
        map<string, int> fileTypesCountMap = getFileTypesCountMap(lines, fileTypesMap);

        for (auto [key, value] : fileTypesCountMap)
        {
            cout << key << ": " << value << endl;
        }
    }
    else
    {
        cout << "Failed to fork" << endl;
        return 1;
    }

    return 0;
}

vector<string> getOutputLines(int pipeDescriptor)
{
    string result;
    char c;

    while (read(pipeDescriptor, &c, sizeof(char)) > 0)
    {
        result.push_back(c);
    }

    vector<string> lines;
    stringstream ss(result);
    string line;

    while (getline(ss, line))
    {
        lines.push_back(line);
    }
    return lines;
}

map<char, string> getAvailableFiletypesMap()
{
    map<char, string> fileTypesMap{
        {'-', "regular file"},
        {'d', "directory"},
        {'l', "symbolic link"},
        {'p', "named pipe"},
        {'c', "character device"},
        {'l', "block device"},
        {'s', "socket"}};

    return fileTypesMap;
}

map<string, int> getFileTypesCountMap(vector<string> lines, map<char, string> availableFileTypes)
{
    map<string, int> fileTypesCountMap;

    lines = vector<string>(lines.begin() + 1, lines.end());
    for (string line : lines)
    {
        auto iter = availableFileTypes.find(line[0]);
        if (iter != availableFileTypes.end())
        {
            fileTypesCountMap[availableFileTypes[line[0]]]++;
        }
        else
        {
            fileTypesCountMap["unknown"]++;
        }
    }

    return fileTypesCountMap;
}