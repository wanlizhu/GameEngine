#pragma once

#include "BasicTypes.h"

struct ModelDesc;

std::string getEnv(const char* name);
void setEnv(const char* name, const char* value);
void split(const std::string& str, const char* seps, std::vector<std::string>* results);
std::string stringFormat(const char* format, ...);
std::string currentPath();
std::string resourcePathWithName(const std::string& name);
std::vector<std::string> resourcePathsWithType(const std::string& type);
glm::ivec2 drawableSizeWithMTKView(void* view);
bool loadBinaryFile(const std::string& path, std::vector<uint8_t>* bytes);
bool loadImageFile(const std::string& path, std::vector<uint32_t>* pixels, int* width, int* height);
bool loadModelFile(const std::string& path, ModelDesc* desc);
