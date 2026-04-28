#include "register_settings.h"
#include <QSettings>
#include <QFile>
#include <QDir>

static char* TAG_NAME{"[ SETTINGS ]"};

std::map<QString,QString>                SettingsRegister::settings;
std::map<QString,float>                  SettingsRegister::settings_values;
std::map<QString,std::pair<float,float>> SettingsRegister::settings_pairs;
std::pair<float,float> SettingsRegister::CAMERA_IMAGE_SIZE{160,160};

bool SettingsRegister::FLAG_SETTINGS_LOADED = false;

void SettingsRegister::LoadSettings(QString path_default)
{
  QString file_path;

  auto CheckPath = [&file_path](QString path) -> bool
  {
    if(path.isEmpty()) return false;

                file_path = path+"/SETTINGS.ini";
     QFile file(file_path);                       qDebug() << "[ CHECK PATH ]" << file_path << file.exists();
                       return file.exists(); 
  };
  FLAG_SETTINGS_LOADED = true;

  //QStringList params; 
  //            params << "/home" << "-maxdepth" << "4" << "-type" << "f" << "-name" << "TrackingSettings.ini";
  //                              QProcess findCommand;
  //                                       findCommand.start("/usr/bin/find",params); 
  //                                       findCommand.waitForFinished(1000);
  //                  QString result_str = findCommand.readAll(); 
  //QStringList result_list = result_str.split("\n");

          path_default = QString("%1/%2").arg(path_default).arg("TrackingProject");
  //QString path_env = qgetenv("TRACKING_SETTINGS_PATH");
  //QString path_current = QDir::currentPath() + "/SETTINGS/TrackingProject/TrackingSettings.ini";
  //QString path_home = QString("/home/orangepi/SETTINGS/TrackingProject/TrackingSettings.ini");
  qDebug() << "LOAD SETTINGS PATH: " << path_default;



         std::vector<QString>   LocationList;
                         
  if(CheckPath(path_default))  LocationList.push_back(file_path);
  //if(CheckPath(path_home)    ) LocationList.push_back(file_path);
  //if(CheckPath(path_current))  LocationList.push_back(file_path);
  //if(CheckPath(result_list[0])) LocationList.push_back(file_path);

  qDebug() << "=================================";
  for(auto& path: LocationList) qDebug() << " EXISTS: " << path;
  qDebug() << "=================================" << Qt::endl;

  bool result = false;
  for(auto& path: LocationList) 
  { result = TryLoadSettings(path, "PATHS"); if(result) break; }

    result = TryLoadSettings(SettingsRegister::GetString("FILE_PORTS"),"PORTS");

  AppendSettings("CAMERA_IMAGE_SIZE", std::pair<float,float>(720.0,540.0));

  qDebug() << "============================";
  qDebug() << "[ LOADED RECORDS ]";
  PrintSettingsList();
  qDebug() << "============================";
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

void SettingsRegister::SetStaticSettings()
{
  if(settings_pairs.contains("CAMERA_IMAGE_SIZE")) SettingsRegister::CAMERA_IMAGE_SIZE = settings_pairs["CAMERA_IMAGE_SIZE"];
}


void SettingsRegister::AppendSettings(QString key, QString setting)                { settings.emplace(key,setting);        SetStaticSettings();}
void SettingsRegister::AppendSettings(QString key, float setting)                  { settings_values.emplace(key,setting); SetStaticSettings();}
void SettingsRegister::AppendSettings(QString key, std::pair<float,float> setting) { settings_pairs.emplace(key,setting);  SetStaticSettings();}

void SettingsRegister::ResetSettings(QString key, QString setting)                { settings[key] = setting; SetStaticSettings();}
void SettingsRegister::ResetSettings(QString key, float setting)                  { settings_values[key] = setting; SetStaticSettings();}
void SettingsRegister::ResetSettings(QString key, std::pair<float,float> setting) { settings_pairs[key] = setting; SetStaticSettings();}

