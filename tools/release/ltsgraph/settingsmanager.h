#pragma once

#include <mcrl2/utilities/logger.h>
#include <QHash>
#include <string>
#include <functional>
#include <memory>

#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QString>
#include <QByteArray>
#include <type_traits>

#include <QSpinBox>
#include <QCheckBox>
#include <QSlider>
#include <QComboBox>
#include <QLineEdit>

template <typename T> struct GetterSetter
{
  template <typename Obj>
  GetterSetter(Obj* obj, const T default_value, const bool reset_to_default)
      : default_value(default_value), reset_to_default(reset_to_default)
  {
    if constexpr (std::is_same_v<Obj, QSpinBox> || std::is_same_v<Obj, QSlider>)
    {
      getter = [obj]() { return obj->value(); };
      setter = [obj, reset_to_default, default_value](T value)
      { obj->setValue(reset_to_default ? default_value : value); };
    }
    else if constexpr (std::is_same_v<Obj, QCheckBox>)
    {
      getter = [obj]() { return obj->isChecked(); };
      setter = [obj, reset_to_default, default_value](T value)
      { obj->setChecked(reset_to_default ? default_value : value); };
    }
    else if constexpr (std::is_same_v<Obj, QComboBox>)
    {
      getter = [obj]() { return obj->currentIndex(); };
      setter = [obj, reset_to_default, default_value](T value)
      { obj->setCurrentIndex(reset_to_default ? default_value : value); };
    }
    else if constexpr (std::is_same_v<Obj, QLineEdit>)
    {
      getter = [obj]() { return obj->text(); };
      setter = [obj, reset_to_default, default_value](T value)
      { obj->setText(reset_to_default ? default_value : value); };
    }
    else
    {
      mCRL2log(mcrl2::log::warning)
          << "[SettingsManager] "
          << "Attempting to create getter/setter for unknown object type: \""
          << typeid(Obj).name() << "\". Skipping this element." << std::endl;
    }
  }

  std::function<T()> getter;
  std::function<void(T)> setter;
  const T default_value;
  const bool reset_to_default;
};

template <typename T>
using varmap = QHash<QString, std::shared_ptr<GetterSetter<T>>>;

class Settings
{
  public:
  Settings(QString class_name) : m_class_name(class_name){};

  template <typename Obj, typename T>
  void registerVar(Obj* obj, const T default_value,
                   bool reset_to_default = false)
  {
    QString name = obj->objectName();
    if constexpr (std::is_same_v<bool, T>)
    {
      if (m_bools.find(name) == m_bools.end())
      {
        m_bools[name] =
            std::make_shared<GetterSetter<bool>>(obj, default_value, reset_to_default);
      }
    }
    else if constexpr (std::is_same_v<int, T>)
    {
      if (m_ints.find(name) == m_ints.end())
      {
        m_ints[name] =
            std::make_shared<GetterSetter<int>>(obj, default_value, reset_to_default);
      }
    }
    else if constexpr (std::is_same_v<float, T>)
    {
      if (m_floats.find(name) == m_floats.end())
      {
        m_floats[name] =
            std::make_shared<GetterSetter<float>>(obj, default_value, reset_to_default);
      }
    }
    else if constexpr (std::is_same_v<QString, T>)
    {
      if (m_strings.find(name) == m_strings.end())
      {
      m_strings[name] =
          std::make_shared<GetterSetter<QString>>(obj, default_value, reset_to_default);
      }
    }
    else
    {
      mCRL2log(mcrl2::log::warning)
          << "[SettingsManager] "
          << "Trying to register variable \"" << name.toStdString()
          << "\" of an unknown type : \"" << typeid(T).name() << "\""
          << std::endl;
      return;
    }

    mCRL2log(mcrl2::log::debug)
        << "[SettingsManager] "
        << "Added \"" << m_class_name.toStdString()
        << "::" << name.toStdString() << "\"" << std::endl;
  }

  void load(const QByteArray& bytes)
  {
    mCRL2log(mcrl2::log::debug) << "[SettingsManager] "
        << "Reading JSON for \"" << m_class_name.toStdString() << "\": \n "
                                << bytes.toStdString() << std::endl;
    m_json = QJsonDocument::fromJson(bytes);
    QJsonObject json_obj = m_json.object();
    _load(m_bools, json_obj);
    _load(m_ints, json_obj);
    _load(m_floats, json_obj);
    _load(m_strings, json_obj);
  }

  QByteArray save()
  {
    QJsonObject json_obj;
    _save(m_bools, json_obj);
    _save(m_ints, json_obj);
    _save(m_floats, json_obj);
    _save(m_strings, json_obj);
    m_json.setObject(json_obj);
    return m_json.toJson();
  }

  private:
  QJsonDocument m_json;
  varmap<bool> m_bools;
  varmap<int> m_ints;
  varmap<float> m_floats;
  varmap<QString> m_strings;
  QString m_class_name;

  template <typename T> void _load(varmap<T>& vars, QJsonObject& json_obj)
  {
    for (auto it = vars.begin(); it != vars.end(); ++it)
    {
      if constexpr (std::is_same_v<bool, T>)
      {
        it.value()->setter(
            json_obj[it.key()].toBool(it.value()->default_value));
      }
      else if constexpr (std::is_same_v<int, T>)
      {
        it.value()->setter(
            json_obj[it.key()].toInt(it.value()->default_value));
      }
      else if constexpr (std::is_same_v<float, T>)
      {
        it.value()->setter(
            json_obj[it.key()].toDouble(it.value()->default_value));
      }
      else if constexpr (std::is_same_v<QString, T>)
      {
        it.value()->setter(
            json_obj[it.key()].toString(it.value()->default_value));
      }
      else
      {
        mCRL2log(mcrl2::log::debug) << "[SettingsManager] "
                                    << "Trying to load an unknown type: \""
                                    << typeid(T).name() << "\"" << std::endl;
      }
    }
  }

  template <typename T> void _save(varmap<T>& vars, QJsonObject& json_obj)
  {
    for (auto it = vars.begin(); it != vars.end(); ++it)
    {
      std::string name = it.key().toStdString();
      T value = it.value()->getter();
      json_obj.insert(it.key(), value);
      mCRL2log(mcrl2::log::debug) << "[SettingsManager] "
          << "Saving: " << name << std::endl;
    }
  }
};

// Singleton settingsmanager class
class SettingsManager
{
  public:
  static SettingsManager& getInstance()
  {
    static SettingsManager instance; // Guaranteed to be destroyed.
                                     // Instantiated on first use.
    return instance;
  }

  private:
  SettingsManager()
  {
  }

  public:
  static Settings* getSettings(QString class_name);
  static void addSettings(QString class_name);

  private:
  QHash<QString, Settings*> m_settings_map;
  Settings* _getSettings(QString class_name);
  void _addSettings(QString class_name);
};