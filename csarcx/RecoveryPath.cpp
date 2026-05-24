#include "pch.h"
#include "RecoveryPath.h"

#include <mutex>
#include <sstream>
#include "xxHash64.h"
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <iostream>
#include "ArchiveUtil.h"

using namespace std;

static unordered_map<uint64_t, string> g_hashMap;

static void Trim(string& s)
{
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == string::npos)
    {
        s.clear();
        return;
    }

    size_t end = s.find_last_not_of(" \t\r\n");
    s = s.substr(start, end - start + 1);
}

void InitMap(const string& dictionary)
{
    istringstream ss(dictionary);
    string line;

    while (getline(ss, line))
    {
        Trim(line);
        if (line.empty())
            continue;

        if (line[0] == '?')
            continue;

        size_t commentPos = line.find('?');
        if (commentPos != string::npos)
        {
            line = line.substr(0, commentPos);
            Trim(line);
            if (line.empty())
                continue;
        }

        string content = line;
        if (line[0] == '[')
        {
            size_t end = line.find(']');
            if (end == string::npos)
                continue;

            size_t pos = end + 1;
            while (pos < line.size() && isspace((unsigned char)line[pos]))
                ++pos;

            if (pos >= line.size())
                continue;

            content = line.substr(pos);
            Trim(content);
        }

        if (content.empty())
            continue;

        // hash -> path
        size_t arrow = content.find("->");
        if (arrow != string::npos)
        {
            string hashStr = content.substr(0, arrow);
            string path = content.substr(arrow + 2);

            Trim(hashStr);
            Trim(path);

            if (path.empty())
                continue;

            try
            {
                uint64_t hash = stoull(hashStr);
                g_hashMap[hash] = path;
            }
            catch (...)
            {
                cout << "无效的 hash: " << hashStr << endl;
            }

            continue;
        }

        string path = content;
        string lower = ArchiveUtil::NormalizeFileName(path);
        uint64_t hash = ArchiveUtil::XXHash64(lower);

        if (g_hashMap.find(hash) == g_hashMap.end())
        {
            g_hashMap[hash] = path;
        }
    }
}

string HashGetPath(uint64_t filePath)
{
    auto it = g_hashMap.find(filePath);
    if (it != g_hashMap.end())
    {
        return it->second;
    }

    return to_string(filePath);
}
