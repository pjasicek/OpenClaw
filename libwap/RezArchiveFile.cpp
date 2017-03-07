#include <vector>
#include <fstream>
#include <algorithm>
#include <string.h>
#include <stdint.h>
#include <mutex>
#include <map>
#include <string>
#include <string.h>
#include <cctype>

#include "libwap.h"
#include <iostream>

using namespace std;

/*************************************************************************/
/**************************** PRIVATE STRUCTURES *************************/
/*************************************************************************/

struct RezArchiveFileEntry
{
    RezArchive* rezArchive;
    std::ifstream* fileStream;
    std::mutex mutex;
};

/*************************************************************************/
/******************** PRIVATE DATA GLOBAL VARIABLES **********************/
/*************************************************************************/

typedef std::vector<RezFile*> RezFileVec;

static std::map<RezArchive*, RezArchiveFileEntry*> g_rezArchiveFileEntryMap;
static std::map<RezFile*, char*> g_rezFileDataMap;
static std::map<RezArchive*, RezFileVec*> g_rezArchiveFilesMap;

uint8_t directorySeparator = '/';

/*************************************************************************/
/*************************** HELPER FUNCTIONS ****************************/
/*************************************************************************/

template<typename T>
static void FileRead(T* dest, std::ifstream* file, int32_t offset)
{
    file->seekg(offset, std::ios::beg);
    file->read((char*)dest, sizeof(*dest));
}

static char* StdStringToCharArray(std::string source)
{
    char* ret = new char[source.length() + 1];
    strcpy(ret, source.c_str());
    return ret;
}

static std::vector<std::string> SplitStringIntoTokens(const char* input, char delim)
{
    uint32_t i;
    uint32_t inputLength = strlen(input);
    std::string token;
    std::vector<std::string> tokensVector;

    // Validity check
    if ((input == NULL) || (inputLength == 0))
    {
        // Return empty vector
        return tokensVector;
    }

    for (i = 0; i < inputLength; i++)
    {
        if (input[i] == delim)
        {
            if (token.empty())
            {
                continue;
            }

            tokensVector.push_back(token);
            token.clear();
            continue;
        }
        token += input[i];
    }

    // Add leftover token if there is any
    if (token.length() > 0)
    {
        tokensVector.push_back(token);
    }

    return tokensVector;
}

void ToLower(char* string)
{
    if (string == NULL)
    {
        return;
    }

    size_t len = strlen(string);
    std::transform(string, string + len, string, (int(*)(int)) std::tolower);
}

/*************************************************************************/
/************************** API IMPLEMENTATIONS **************************/
/*************************************************************************/

char* WAP_GetRezFileData(RezFile* rezFile)
{
    // Check if we got valid input
    if ((rezFile == NULL) || (rezFile->owner == NULL))
    {
        return NULL;
    }

    // Check if we already accessed this file
    if (g_rezFileDataMap.count(rezFile) != 0)
    {
        return g_rezFileDataMap[rezFile];
    }
    else
    {
        // First time accessing it, we have to allocate it and load it

        // Check if there is REZ archive entry
        if (g_rezArchiveFileEntryMap.count(rezFile->owner) == 0)
        {
            return NULL;
        }

        // Create new entry for rez file and allocate its data buffer
        g_rezFileDataMap.insert(std::pair<RezFile*, char*>(rezFile, new char[rezFile->size]));

        RezArchiveFileEntry* rezArchiveFileEntry = g_rezArchiveFileEntryMap[rezFile->owner];

        std::lock_guard<std::mutex> lock(rezArchiveFileEntry->mutex);

        // Seek to file's offset within REZ file and load it
        rezArchiveFileEntry->fileStream->seekg(rezFile->offset, std::ios::beg);
        rezArchiveFileEntry->fileStream->read(g_rezFileDataMap[rezFile], rezFile->size);
    }

    return g_rezFileDataMap[rezFile];
}

void WAP_FreeFileData(RezFile* rezFile)
{
    // Check validity
    if (rezFile == NULL)
    {
        return;
    }

    // Check if file data for this REZ file are loaded
    if (g_rezFileDataMap.count(rezFile) == 0)
    {
        // Nothing to do
        return;
    }

    delete[] g_rezFileDataMap[rezFile];
    g_rezFileDataMap[rezFile] = NULL;
    g_rezFileDataMap.erase(rezFile);
}

static RezFile* GetChildFile(RezDirectory* rezFileDirectory, std::string fileName)
{
    uint32_t i;
    RezFile* searchedFile = NULL;

    // Check validity
    if ((rezFileDirectory == NULL) || (rezFileDirectory->directoryContents == NULL) ||
        (fileName.length() == 0))
    {
        return NULL;
    }

    for (i = 0; i < rezFileDirectory->directoryContents->rezFilesCount; i++)
    {
        std::string rezFileName(rezFileDirectory->directoryContents->rezFiles[i]->name);
        rezFileName.append(".").append(rezFileDirectory->directoryContents->rezFiles[i]->extension);
        if (fileName.compare(rezFileName) == 0)
        {
            searchedFile = rezFileDirectory->directoryContents->rezFiles[i];
            break;
        }
    }

    return searchedFile;
}

static RezDirectory* GetChildDirectory(RezDirectory* currentDirectory, std::string& searchedDirectoryName)
{
    uint32_t i;
    RezDirectory* searchedDirectory = NULL;

    // Check validity
    if (currentDirectory == NULL)
    {
        return NULL;
    }

    for (i = 0; i < currentDirectory->directoryContents->rezDirectoriesCount; i++)
    {
        if (strcmp(currentDirectory->directoryContents->rezDirectories[i]->name, searchedDirectoryName.c_str()) == 0)
        {
            searchedDirectory = currentDirectory->directoryContents->rezDirectories[i];
            break;
        }
    }

    return searchedDirectory;
}

RezDirectory* WAP_GetRezDirectoryFromRezArchive(RezArchive* rezArchive, const char* rezDirectoryPath)
{
    // Check if we got valid input
    if (rezArchive == NULL)
    {
        return NULL;
    }

    // If given path to searched directory is NULL or empty, return root directory
    if ((rezDirectoryPath == NULL) || (strcmp(rezDirectoryPath, "") == 0))
    {
        return rezArchive->rootDirectory;
    }

    return WAP_GetRezDirectoryFromRezDirectory(rezArchive->rootDirectory, rezDirectoryPath);
}

RezDirectory* WAP_GetRezDirectoryFromRezDirectory(RezDirectory* rezDirectory, const char* rezDirectoryPath)
{
    // Check validity
    if (rezDirectory == NULL)
    {
        return NULL;
    }

    // Transform directory path to lower case
    std::string path = rezDirectoryPath;
    std::transform(path.begin(), path.end(), path.begin(), (int(*)(int)) std::tolower);

    // Get path to searched directory in form of tokens, each representing directory
    std::vector<std::string> directoriesTokens = SplitStringIntoTokens(path.c_str(), directorySeparator);
    if (directoriesTokens.empty())
    {
        return rezDirectory;
    }

    RezDirectory* currentDirectory = rezDirectory;
    for (auto rezDirectoryName : directoriesTokens)
    {
        if (currentDirectory == NULL)
        {
            return NULL;
        }

        currentDirectory = GetChildDirectory(currentDirectory, rezDirectoryName);
    }

    return currentDirectory;
}

RezFile* WAP_GetRezFileFromRezArchive(RezArchive* rezArchive, const char* rezFilePath)
{
    static std::mutex mutex;

    // Check if we got valid input
    if ((rezArchive == NULL) || (rezArchive->rootDirectory == NULL) ||
        (rezFilePath == NULL) || (strlen(rezFilePath) == 0))
    {
        return NULL;
    }

    return WAP_GetRezFileFromRezDirectory(rezArchive->rootDirectory, rezFilePath);

    //--- Remnants of the past, possible optimalization for the future, saves ~3 microseconds upon each lookup

    // We always start from root. Make it easier for users to be able to
    // specify path both as "LEVEL1/IMAGES/ASDF.PID" same as "/LEVEL1/IMAGES/ASDF.PID"
    /*std::string fileKey(rezFilePath);
    if (fileKey[0] != directorySeparator)
    {
        fileKey.insert(0, 1, directorySeparator);
    }
    
    // Critical section
    std::lock_guard<std::mutex> lock(mutex);

    if (g_rezArchiveFileMap.count(rezArchive) == 0)
    {
        return NULL;
    }

    return (*g_rezArchiveFileMap[rezArchive])[std::string(fileKey)];*/
}

RezFile* WAP_GetRezFileFromRezDirectory(RezDirectory* rezDirectory, const char* rezFilePath)
{
    // Check if we got valid input
    if ((rezDirectory == NULL) || (rezDirectory->directoryContents == NULL) ||
        (rezFilePath == NULL) || (strlen(rezFilePath) == 0))
    {
        return NULL;
    }

    std::string fullFilePath(rezFilePath);

    // Transform file path to lower case
    std::transform(fullFilePath.begin(), fullFilePath.end(), fullFilePath.begin(), (int(*)(int)) std::tolower);

    size_t fileNamePos = fullFilePath.find_last_of(directorySeparator);
    // In case rezFilePath is only filename, like CLAW.PID and not /CLAW/IMAGE/CLAW.PID
    if (fileNamePos == std::string::npos)
    {
        return GetChildFile(rezDirectory, std::string(fullFilePath.c_str()));
    }

    std::string fullFileName = fullFilePath.substr(fileNamePos + 1);
    std::string directoryPath = fullFilePath.substr(0, fileNamePos);

    // Try to get directory in which searched file resides
    RezDirectory* searchedFileDirectory = WAP_GetRezDirectoryFromRezDirectory(rezDirectory, directoryPath.c_str());
    if (searchedFileDirectory == NULL)
    {
        return NULL;
    }

    // Try to retrieve file from its directory
    return GetChildFile(searchedFileDirectory, fullFileName);
}

static void RegisterRezArchiveFile(RezArchive* rezArchive, std::ifstream* rezArchiveFileStream)
{
    // Create loaded REZ file entry
    RezArchiveFileEntry* rezArchiveFileEntry = new RezArchiveFileEntry;
    rezArchiveFileEntry->rezArchive = rezArchive;
    rezArchiveFileEntry->fileStream = rezArchiveFileStream;

    g_rezArchiveFileEntryMap.insert(std::pair<RezArchive*, RezArchiveFileEntry*>(rezArchive, rezArchiveFileEntry));
}

static void UnregisterRezArchiveFile(RezArchive* rezArchive)
{
    // First check if there is given rez archive registered
    if (g_rezArchiveFileEntryMap.count(rezArchive) != 0)
    {
        // Unregister loaded REZ file entry
        RezArchiveFileEntry* rezArchiveFileEntry = g_rezArchiveFileEntryMap[rezArchive];
        delete rezArchiveFileEntry->fileStream;
        delete rezArchiveFileEntry;
        rezArchiveFileEntry = NULL;
        g_rezArchiveFileEntryMap.erase(rezArchive);
    }
}

static std::string GetRezFileFullPath(RezFile* rezFile)
{
    if (rezFile == NULL)
    {
        return "";
    }

    std::string dirPath;

    RezDirectory* parentDir = rezFile->parent;
    while (parentDir != NULL)
    {
        dirPath = std::string(parentDir->name) + (char)directorySeparator + dirPath;
        parentDir = parentDir->parent;
    }

    std::string fullPath = dirPath + rezFile->name + '.' + rezFile->extension;

    return fullPath;
}

void FillRezFileMapWithDirectoryFiles(RezFileVec*& rezFileVec, RezDirectory* rezDirectory)
{
    if (rezDirectory->directoryContents == NULL)
    {
        return;
    }
    
    for (uint32_t fileIdx = 0; fileIdx < rezDirectory->directoryContents->rezFilesCount; fileIdx++)
    {
        RezFile* rezFile = rezDirectory->directoryContents->rezFiles[fileIdx];

        rezFileVec->push_back(rezFile);
    }

    for (uint32_t dirIdx = 0; dirIdx < rezDirectory->directoryContents->rezDirectoriesCount; dirIdx++)
    {
        FillRezFileMapWithDirectoryFiles(rezFileVec, rezDirectory->directoryContents->rezDirectories[dirIdx]);
    }
}

static void CreateRezArchiveFileMap(RezArchive* rezArchive)
{
    if (g_rezArchiveFilesMap.count(rezArchive) != 0)
    {
        return;
    }

    RezFileVec* rezFileVec = new RezFileVec;
    g_rezArchiveFilesMap.insert(std::make_pair(rezArchive, rezFileVec));

    RezDirectory* rootDirectory = rezArchive->rootDirectory;
    
    FillRezFileMapWithDirectoryFiles(rezFileVec, rootDirectory);
}

void DestroyRezArchiveFileMap(RezArchive* rezArchive)
{
    if (g_rezArchiveFilesMap.count(rezArchive) == 0)
    {
        return;
    }

    RezFileVec* rezFileVec = g_rezArchiveFilesMap[rezArchive];

    rezFileVec->clear();
    delete rezFileVec;
    g_rezArchiveFilesMap.erase(rezArchive);
    rezFileVec = NULL;
}

RezFile* WAP_GetRezFileFromFileIdx(RezArchive* rezArchive, uint32_t rezFileIdx)
{
    if (g_rezArchiveFilesMap.count(rezArchive) == 0)
    {
        return NULL;
    }

    return g_rezArchiveFilesMap[rezArchive]->at(rezFileIdx);
}

uint32_t WAP_GetRezFilesCount(RezArchive* rezArchive)
{
    if (g_rezArchiveFilesMap.count(rezArchive) == 0)
    {
        return 0;
    }

    return g_rezArchiveFilesMap[rezArchive]->size();
}

static void ReadRezDirectory(RezArchive*& rezArchive, RezDirectory* rezDirectory, std::ifstream* fileStream)
{
    uint32_t i;
    uint32_t currentOffset = rezDirectory->offset;
    // Keeps track of how many more bytes need to be read in this directory - it
    // specifies how much more of files/directories are remaining to be read
    int32_t remainingBytes = (int32_t)rezDirectory->size;

    // Vectors which temporarily store loaded rez directories and files
    std::vector<RezDirectory*> loadedRezDirectories;
    std::vector<RezFile*> loadedRezFiles;

    while (remainingBytes > 0)
    {
        uint32_t bytesRead = 0;
        uint32_t isDirectoryFlag;
        // This directory contains something, make sure directory contents are allocated
        if (rezDirectory->directoryContents == NULL)
        {
            rezDirectory->directoryContents = new RezDirectoryContents;
            // Initialize to default values
            (*rezDirectory->directoryContents) = { 0 };
        }
        // Determine whether next element is file or another directory
        FileRead(&isDirectoryFlag, fileStream, currentOffset);
        if (isDirectoryFlag)
        {
            // Create new rez directory
            RezDirectory* newRezDirectory = new RezDirectory;
            (*newRezDirectory) = { 0 };
            newRezDirectory->parent = rezDirectory;

            loadedRezDirectories.emplace_back(newRezDirectory);

            FileRead(&(newRezDirectory->offset),      fileStream, currentOffset + 4);
            FileRead(&(newRezDirectory->size),        fileStream, currentOffset + 8);
            FileRead(&(newRezDirectory->dateAndTime), fileStream, currentOffset + 12);

            // Move cursor to directory name's offset
            fileStream->seekg(currentOffset + 16, std::ios::beg);
            std::string directoryName("");
            // Read directory name. Directory name is terminated by null character
            while (true)
            {
                char c = fileStream->get();
                if (c == 0)
                {
                    break;
                }
                directoryName += c;
            }
            // Convert C++ string to char*
            newRezDirectory->name = StdStringToCharArray(directoryName);
            // Convert to lowercase to maintain consistency
            ToLower(newRezDirectory->name);

            bytesRead = 16 + directoryName.length() + 1;
            remainingBytes -= bytesRead;
            currentOffset += bytesRead;

            // Recursively read all directories
            ReadRezDirectory(rezArchive, newRezDirectory, fileStream);
        }
        else
        {
            uint32_t unk;

            // Create new rez file
            RezFile* newRezFile = new RezFile;
            (*newRezFile) = { 0 };
            newRezFile->parent = rezDirectory;
            newRezFile->owner = rezArchive;

            loadedRezFiles.emplace_back(newRezFile);

            FileRead(&(newRezFile->offset), fileStream, currentOffset + 4);
            FileRead(&(newRezFile->size), fileStream, currentOffset + 8);
            FileRead(&(newRezFile->dateAndTime), fileStream, currentOffset + 12);
            FileRead(&(newRezFile->fileId), fileStream, currentOffset + 16);
            // Extension in reverse order
            fileStream->read(newRezFile->extension, 4);
            std::reverse(newRezFile->extension, newRezFile->extension + strlen(newRezFile->extension));
            // Unknown dummy value
            FileRead(&unk, fileStream, currentOffset + 24);

            fileStream->seekg(currentOffset + 28, std::ios::beg);
            std::string fileName("");
            // Read file name. File name is terminated by null character
            while (true)
            {
                char c = fileStream->get();
                if (c == 0)
                {
                    break;
                }
                fileName += c;
            }
            // Bare file name
            newRezFile->name = StdStringToCharArray(fileName);

            // Full filename with path, name and file extension
            std::string fileFullPath = GetRezFileFullPath(newRezFile);
            newRezFile->fullPathAndName = StdStringToCharArray(fileFullPath);

            // Random NULL char
            char c = fileStream->get();
            
            bytesRead = 28 + fileName.length() + 1 + 1;
            remainingBytes -= bytesRead;
            currentOffset += bytesRead;

            // Convert everything to lower case to maintain consistency
            ToLower(newRezFile->name);
            ToLower(newRezFile->extension);
            ToLower(newRezFile->fullPathAndName);
        }
    }

    // Dont save anything to empty directories
    if (rezDirectory->directoryContents == NULL)
    {
        return;
    }

    // Allocate memory for loaded rez directories
    rezDirectory->directoryContents->rezDirectoriesCount = loadedRezDirectories.size();
    rezDirectory->directoryContents->rezDirectories = new RezDirectory*[loadedRezDirectories.size()];

    // Store loaded rez directories
    i = 0;
    for (auto rezDirectoryIter : loadedRezDirectories)
    {
        rezDirectory->directoryContents->rezDirectories[i] = rezDirectoryIter;
        i++;
    }

    // Allocate memory for loaded rez files
    rezDirectory->directoryContents->rezFilesCount = loadedRezFiles.size();
    rezDirectory->directoryContents->rezFiles = new RezFile*[loadedRezFiles.size()];

    // Store loaded rez files
    i = 0;
    for (auto rezFileIter : loadedRezFiles)
    {
        rezDirectory->directoryContents->rezFiles[i] = rezFileIter;
        i++;
    }
}

RezArchive* WAP_LoadRezArchive(const char* rezFilePath)
{
    std::ifstream* fileStream = new std::ifstream(rezFilePath, std::ifstream::binary);
    if (!fileStream->is_open())
    {
        return NULL;
    }

    RezArchive* rezArchive = new RezArchive;
    rezArchive->rootDirectory = new RezDirectory;
    // Initialize to default values
    (*rezArchive->rootDirectory) = { 0 };
    rezArchive->rootDirectory->name = StdStringToCharArray(std::string(""));

    // Read rez file header which is 127 bytes long
    fileStream->read(rezArchive->header, 127);
    // Read rez file version
    FileRead(&(rezArchive->version), fileStream, 127);
    // Read rez payload offset (offset of first directory)
    FileRead(&(rezArchive->rootDirectory->offset), fileStream, 131);
    // 
    FileRead(&(rezArchive->rootDirectory->size), fileStream, 135);

    // Checksum, offset to last archive + its node size should == file size
    uint32_t expectedRezArchiveSize = rezArchive->rootDirectory->offset + rezArchive->rootDirectory->size;
    // Get actual size of file we opened
    fileStream->seekg(0, std::ios::end);
    std::streamoff actualLoadedFileSize = fileStream->tellg();
    // If this check fails, we did not load valid REZ file
    if (expectedRezArchiveSize != actualLoadedFileSize)
    {
        WAP_DestroyRezArchive(rezArchive);
        return NULL;
    }

    // Recursively read all directories
    ReadRezDirectory(rezArchive, rezArchive->rootDirectory, fileStream);

    // Register loaded REZ archive file
    RegisterRezArchiveFile(rezArchive, fileStream);

    // Create map of REZ files with key being their full file path
    //START_QUERY_PERFORMANCE_TIMER;
    CreateRezArchiveFileMap(rezArchive);
    //END_QUERY_PERFORMANCE_TIMER;
    return rezArchive;
}

static void DestroyRezDirectory(RezDirectory* rezDirectory)
{
    // Directory name is always set, delete
    delete[] rezDirectory->name;
    // Skip if there is nothing in this directory
    if (rezDirectory->directoryContents == NULL)
    {
        return;
    }

    // Recursively delete all sub directories
    for (int i = 0; i < rezDirectory->directoryContents->rezDirectoriesCount; i++)
    {
        DestroyRezDirectory(rezDirectory->directoryContents->rezDirectories[i]);
        delete rezDirectory->directoryContents->rezDirectories[i];
    }
    delete[] rezDirectory->directoryContents->rezDirectories;

    // Delete all files
    for (int i = 0; i < rezDirectory->directoryContents->rezFilesCount; i++)
    {
        WAP_FreeFileData(rezDirectory->directoryContents->rezFiles[i]);
        delete[] rezDirectory->directoryContents->rezFiles[i]->fullPathAndName;
        delete[] rezDirectory->directoryContents->rezFiles[i]->name;
        delete rezDirectory->directoryContents->rezFiles[i];
    }
    delete[] rezDirectory->directoryContents->rezFiles;

    // Delete directory contents metadata
    delete rezDirectory->directoryContents;
}

// Valgrind - "All heap blocks were freed -- no leaks are possible"
void WAP_DestroyRezArchive(RezArchive* rezArchive)
{
    // Unregister loaded REZ file entry
    UnregisterRezArchiveFile(rezArchive);

    DestroyRezDirectory(rezArchive->rootDirectory);

    // Clear file map associated with this archive
    DestroyRezArchiveFileMap(rezArchive);

    delete rezArchive->rootDirectory;
    delete rezArchive;
}

LIBWAP_API void WAP_SetDirectorySeparator(uint8_t separator)
{
    directorySeparator = separator;
}

LIBWAP_API uint8_t WAP_GetDirectorySeparator()
{
    return directorySeparator;
}