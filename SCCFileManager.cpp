#include "SCCFileManager.h"
#include "SCCLog.h"

#include <iostream>
#include <fstream>

SCCFileManager::SCCFileManager(const std::string& filename)
{
    //ctor
    m_ssFile << filename;
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

    bool res = readFile(fileName, container);

    return res;
}

bool SCCFileManager::readFile(const std::string& filename, std::string& container)
{
    std::ifstream f;

    if (filename == "")
        return false;

    f.open(filename.c_str(), std::ios::in);

    if (!f.is_open())
        return false;

    f.seekg(0, std::ifstream::end);
    size_t fileSize = f.tellg();
    if (fileSize == 0)
        return false;

    f.seekg(0,std::ifstream::beg);

    char buffer[fileSize+1];

    f.read(buffer, fileSize);
    f.close();

    buffer[fileSize] = NULL_CHAR;

    container = buffer;
    return true;
}

bool SCCFileManager::writeFile(const std::string& filepath, const std::string& filename, const std::string& container)
{
    std::string fileName(filepath);
    fileName += "/";
    fileName += filename;

    bool res = writeFile(fileName, container);

    return res;
}

bool SCCFileManager::writeFile(const std::string& filename, const std::string& container)
{
    std::ofstream f;

    if (filename == "")
        return false;

    f.open(filename.c_str(), std::ios::out);

    if (!f.is_open())
        return false;

    f.write(container.c_str(), container.length());

    bool res = f.good();

    f.close();

    return res;
}

bool SCCFileManager::writeFile(const std::string& container)
{
    std::string filename = m_ssFile.str();

    return writeFile(filename, container);
}

std::string SCCFileManager::getTempFileName()
{
    std::string strFile("tmp_");
    strFile += "1234";
    strFile += ".json";
    return strFile;
}
