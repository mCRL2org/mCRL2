#include <settingsmanager.h>

void SettingsManager::addSettings(QString class_name)
{
  return SettingsManager::getInstance()._addSettings(class_name);
}

void SettingsManager::_addSettings(QString class_name)
{
  if (m_settings_map.find(class_name) == m_settings_map.end())
  {
    mCRL2log(mcrl2::log::debug) << "[SettingsManager] "
                                << "Added \"" << class_name.toStdString()
                                << "\"" << std::endl;
    m_settings_map[class_name] = new Settings(class_name);
  }
  else
  {
    mCRL2log(mcrl2::log::warning)
        << "[SettingsManager] "
        << "Trying to add \"" << class_name.toStdString()
        << "\" This key already exists." << std::endl;
  }
}

// public interface function
Settings* SettingsManager::getSettings(QString class_name)
{
  return SettingsManager::getInstance()._getSettings(class_name);
}

// private function
Settings* SettingsManager::_getSettings(QString class_name)
{
  if (m_settings_map.find(class_name) == m_settings_map.end())
  {
    mCRL2log(mcrl2::log::warning)
        << "[SettingsManager] "
        << "Attempting to retrieve \"" << class_name.toStdString()
        << "\" but it is unknown." << std::endl;
    return nullptr;
  }
  else
  {
    return m_settings_map[class_name];
  }
}
