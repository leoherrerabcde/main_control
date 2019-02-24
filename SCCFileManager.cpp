#include "SCCFileManager.h"

#include <iostream>
#include <fstream>

SCCFileManager::SCCFileManager()
{
    //ctor
}

SCCFileManager::~SCCFileManager()
{
    //dtor
}

bool SCCFileManager::readFile(const std::string& filepath, const std::string& filename, std::string& container)
{
    std::string fileName(filepath);
    fileName += "/";
    fileName += filename;

    readFile(fileName, container);

    return res;
}

bool SCCFileManager::readFile(const std::string& filename, std::string& container)
{
    std::ifstream f;

    if (filename == "")
        return false;

    f.open(filename.c_str(), ios::in);

    if (!f.is_open())
        return false;

    f.seekg(0,ifstream::end);
    size_t fileSize = f.tellg();
    if (fileSize == 0)
        return false;

    f.seekg(0,ifstream::beg);

    char buffer[fileSize+1];

    f.read(buffer, fileSize);
    f.close();

    buffer[fileSize] = NULL_CHAR;

    container = buffer;
    return true;
}

bool SCCFileManager::writeFile(const std::string& filepath, const std::string& filename, std::string& container)
{
    std::string fileName(filepath);
    fileName += "/";
    fileName += filename;

    writeFile(fileName, container);

    return res;
}

bool SCCFileManager::writeFile(const std::string& filename, std::string& container)
{
    std::ofstream f;

    if (filename == "")
        return false;

    f.open(filename.c_str(), ios::out);

    if (!f.is_open())
        return false;

    f.write(container.c_str(), container.length());

    bool res = f.good();

    f.close();

    return res;
}

