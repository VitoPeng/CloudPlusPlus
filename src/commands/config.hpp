#ifndef CONFIG_H
#define CONFIG_H

#include <string>

typedef struct config {
  std::string user;
  std::string password;
  std::string address;
  std::string directory;
} Config;

int config(int argc, char *argv[]);

void save_config(Config *configuration);
Config get_config();

#endif