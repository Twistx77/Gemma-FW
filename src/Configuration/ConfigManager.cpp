#include "ConfigManager.h"

nvs_handle_t ConfigManager::nvsHandle;

bool ConfigManager::initialize()
{
  // Initialize NVS
  esp_err_t err = nvs_flash_init();

  if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
  {
    // NVS partition was truncated and needs to be erased
    // Retry nvs_flash_init
    ESP_ERROR_CHECK(nvs_flash_erase());
    err = nvs_flash_init();
  }

  ESP_ERROR_CHECK(err);
  // Open the NVS namespace "ConfigParams"
  err = nvs_open("ConfigParams", NVS_READWRITE, &ConfigManager::nvsHandle);
  if (err != ESP_OK)
  {
    // Handle the error
    return false;
  }

  // Check if all config parameters are present in the NVS if not, add them with the default value
  for (int i = 0; i < sizeof(DefaultConfigParameters) / sizeof(ConfigParameter); i++)
  {
    uint32_t value;
    esp_err_t err = nvs_get_u32(ConfigManager::nvsHandle, DefaultConfigParameters[i].key, &value);
    if (err != ESP_OK)
    {
      // Set the default value
      err = nvs_set_u32(ConfigManager::nvsHandle, DefaultConfigParameters[i].key, DefaultConfigParameters[i].defaultValue);
      if (err != ESP_OK)
      {
        // Handle the error
        return false;
      }
    }
  }
  return true;
}

uint32_t ConfigManager::getParameter(ConfigParameter parameter)
{
  uint32_t value;

  esp_err_t err = nvs_get_u32(ConfigManager::nvsHandle, parameter.key, &value);
  if (err != ESP_OK)
  {
    // Return the default value if the parameter is not found
    return parameter.defaultValue;
  }
  return value;
}

bool ConfigManager::setParameter(ConfigParameter parameter, uint32_t value)
{
  // Check if the value is within the limits
  if (value < parameter.minValue || value > parameter.maxValue)
  {
    return false;
  }
  esp_err_t err = nvs_set_u32(ConfigManager::nvsHandle, parameter.key, value);
  if (err != ESP_OK)
  {
    // Handle the error
    return false;
  }
  // Commit the changes to flash storage
  err = nvs_commit(ConfigManager::nvsHandle);
  if (err != ESP_OK)
  {
    // Handle the error
    return false;
  }
  return true;
}
