#ifndef _FILESYSTEM_UTILS_H
#define _FILESYSTEM_UTILS_H

#include <sys/stat.h>
#include <string>

static bool fileExists (const std::string& name) {
  struct stat buffer;   
  return (stat (name.c_str(), &buffer) == 0); 
}

#endif