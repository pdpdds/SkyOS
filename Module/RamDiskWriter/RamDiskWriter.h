#pragma once

#include <fstream>
#include <stdint.h>
#include <list>

#define VERSION_MAJOR	1
#define	VERSION_MINOR	0

/**
 *
 */
class SkyRamDisk {
private:
	int idCounter;
	std::ofstream out;
	std::list<std::string> ignores;

	void WriteRecursive(const char* basePath, const char* path, const char* name, uint32_t contentLength, uint32_t parentId, bool isFile);

public:
	SkyRamDisk() :
			idCounter(0) {
	}

	void Create(const char* sourcePath, const char* targetPath);
};

