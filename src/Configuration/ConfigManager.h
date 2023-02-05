#ifndef CONFIG_MANAGER_H_
#define CONFIG_MANAGER_H_

#include <nvs.h>
#include <nvs_flash.h>

#include "ConfigParameters.h"

class ConfigManager
{
private:
  ConfigManager() = default;
  static nvs_handle_t nvsHandle;

public:
  //ConfigManager(const ConfigManager&) = delete;
  //ConfigManager& operator=(const ConfigManager&) = delete;

  static ConfigManager& getInstance()
  {
    static ConfigManager instance;
    return instance;
  }

  ~ConfigManager() = default;

  // Initialize the configuration manager
  bool initialize();
  // Get a specific parameter value
  uint32_t getParameter(ConfigParameter parameter);
  // Set a specific parameter value
  bool setParameter(ConfigParameter parameter, uint32_t value);


};

#endif // CONFIG_MANAGER_H_
