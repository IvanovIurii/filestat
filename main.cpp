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
    int pfd[2];
    pipe(pfd);

    pid_t pid = fork();

    if (pid == 0)
    {
        dup2(pfd[1], STDOUT_FILENO);
        execl("/bin/ls", "/bin/ls", "-l", ".", NULL);

        perror("Can not execute 'ls' command");
        return 1;
    }
    else if (pid > 0)
    {
        wait(NULL);
        close(pfd[1]);

        vector<string> lines = getOutputLines(pfd[0]);
        map<char, string> fileTypesMap = getAvailableFiletypesMap();

        map<string, int> fileTypesCountMap = getFileTypesCountMap(lines, fileTypesMap);
        for (auto [key, value] : fileTypesCountMap)
        {
            cout << key << ": " << value << endl;
        }
    }
    else
    {
        perror("Error to fork");
        return 1;
    }

    return 0;
}

vector<string> getOutputLines(int pipeDescriptor)
{
    string result;
    char buffer[1];

    while (read(pipeDescriptor, buffer, 1) > 0)
    {
        result.push_back(char(*buffer));
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