#include "register_settings.h"
#include <QSettings>
#include <QFile>
#include <QDir>

static char* TAG_NAME{"[ SETTINGS ]"};

std::map<QString,QString>                SettingsRegister::settings;
std::map<QString,float>                  SettingsRegister::settings_values;
std::map<QString,std::pair<float,float>> SettingsRegister::settings_pairs;

bool SettingsRegister::FLAG_SETTINGS_LOADED = false;

void SettingsRegister::LoadSettings()
{
  QString last_path;

  auto CheckPath = [&last_path](QString path) -> bool
  {
    QFile file(path);  last_path = path;

     if(path.isEmpty()) return false;
    if(!path.contains("TrackingSettings.ini")) return false;
    return file.exists(); 
  };
  FLAG_SETTINGS_LOADED = true;

  QStringList params; 
              params << "/home" << "-maxdepth" << "4" << "-type" << "f" << "-name" << "TrackingSettings.ini";

                                QProcess findCommand;
                                         findCommand.start("/usr/bin/find",params); 
                                         findCommand.waitForFinished(1000);
                    QString result_str = findCommand.readAll(); 
  QStringList result_list = result_str.split("\n");

  QString env_Path = qgetenv("TRACKING_SETTINGS_PATH");
  QString currentPath = QDir::currentPath() + "/DATA/TrackingProject/TrackingSettings.ini";
  //QString homePath = QString("/home/orangepi/DATA/TrackingProject/TrackingSettings.ini");
  QString homePath = QString("/home/broms/DATA/TrackingProject/TrackingSettings.ini");



         std::vector<QString>   LocationList;
  if(CheckPath(homePath)   )    LocationList.push_back(last_path);
  //if(CheckPath(currentPath))    LocationList.push_back(last_path);
  //if(CheckPath(result_list[0])) LocationList.push_back(last_path);

  qDebug() << "=================================";
  for(auto& path: LocationList) qDebug() << " EXISTS: " << path;
  qDebug() << "=================================" << Qt::endl;

  bool result = false;
  for(auto& path: LocationList) 
  { result = TryLoadSettings(path, "PATHS"); if(result) break; }

    result = TryLoadSettings(SettingsRegister::GetString("FILE_PORTS"),"PORTS");

  AppendSettings("CAMERA_IMAGE_POS",  std::pair<float,float>(720/2 -80,540/2 -80));
  AppendSettings("CAMERA_IMAGE_SIZE", 160.0);
  AppendSettings("CAMERA_IMAGE_SIZE", std::pair<float,float>(160.0,160.0));


  qDebug() << "============================";
  qDebug() << "[ LOADED RECORDS ]";
  PrintSettingsList();
  qDebug() << "============================";
  //===========================================
}

bool SettingsRegister::TryLoadSettings(QString file, QString GROUP)
{
  if(!QFile::exists(file)) return false;;

  QSettings base_settings(file, QSettings::IniFormat);

            base_settings.beginGroup(GROUP);
                  for(auto key: base_settings.allKeys())
                  settings.emplace(key, base_settings.value(key).toString() );
            base_settings.endGroup();

  qDebug() << TAG_NAME << "LOADED: " << file << "GROUP: " << GROUP;
  return true;
}

void SettingsRegister::AppendSettings(QString key, QString setting)                { settings.emplace(key,setting); }
void SettingsRegister::AppendSettings(QString key, float setting)                  { settings_values.emplace(key,setting); }
void SettingsRegister::AppendSettings(QString key, std::pair<float,float> setting) { settings_pairs.emplace(key,setting); }

void SettingsRegister::ResetSettings(QString key, QString setting)                { settings[key] = setting; }
void SettingsRegister::ResetSettings(QString key, float setting)                  { settings_values[key] = setting; }
void SettingsRegister::ResetSettings(QString key, std::pair<float,float> setting) { settings_pairs[key] = setting; }

