#include "SettingsEngine.h"
#include <QSettings>
#include <QFile>
#include <QDir>

static char* TAG{"[ SETTINGS ]"};

std::map<QString,QString> SettingsPath::settings;
std::map<QString,QString> PortsSettings::settings;

void SettingsPath::LoadSettings()
{
  QString last_path;
  auto CheckPath = [&last_path](QString path) -> bool
  {
    QFile file(path);  

    last_path = path;
    if(path.isEmpty()) return false;
    if(!path.contains("TrainerSettings.ini")) return false;
    return file.exists(); 
  };
  QString env_Path = qgetenv("KLP_SETTINGS_PATH");

  std::vector<QString> settingsPathList;
  if(CheckPath(env_Path)) settingsPathList.push_back(last_path);
  if(CheckPath(QDir::currentPath() + "/DATA/TrainerData/TrainerSettings.ini")) settingsPathList.push_back(last_path);
  if(CheckPath(QString("/home/broms/DATA/TrainerData/TrainerSettings.ini"))) settingsPathList.push_back(last_path);
  if(CheckPath(QString("/home/broms/BACKUP/DATA/TrainerData/TrainerSettings.ini"))) settingsPathList.push_back(last_path);
  if(CheckPath(QString("/home/administrator/DATA/TrainerData/TrainerSettings.ini"))) settingsPathList.push_back(last_path);
  if(CheckPath(QString("/home/spp/DATA/TrainerData/TrainerSettings.ini"))) settingsPathList.push_back(last_path);
  
  QProcess findCommand;
  QStringList params; params << "/home" << "-maxdepth" << "4" << "-type" << "f" << "-name" << "TrainerSettings.ini";
  findCommand.start("/usr/bin/find",params); findCommand.waitForFinished(1000);
  QString result_str = findCommand.readAll(); QStringList result_list = result_str.split("\n");
  if(CheckPath(result_list[0])) settingsPathList.push_back(last_path);


  qDebug() << "=================================" << Qt::endl;
  for(auto& path: settingsPathList) qDebug() << "SETTINGS EXISTS: " << path;
  qDebug() << Qt::endl << "=================================";

  bool result = false;
  for(auto& path: settingsPathList)
  {
  result = TryLoadSettings(path);    if(result) return; qDebug() << TAG << "WARNING: LOAD" << path;
  }
  //===========================================
}

bool SettingsPath::TryLoadSettings(QString file)
{
  qDebug() << "SETTINGS PATH TRY LOAD: " << file;
  QSettings base_settings(file, QSettings::IniFormat);
  base_settings.beginGroup("PATHS");
        for(auto key: base_settings.allKeys())
        settings.emplace(key, base_settings.value(key).toString() );
  base_settings.endGroup();

  PrintSettingsList();

  qDebug() << TAG << "COMMON SETTINGS LOADED: " << file;
  return true;
}



bool PortsSettings::TryLoadSettings(QString file)
{
  if(!QFile(file).exists()) 
  {
    qDebug() << TAG << "NOT EXISTS: " << file;
    return false;
  }

  QSettings base_settings(file, QSettings::IniFormat);

  base_settings.beginGroup("PATHS");
      auto PORTS_FILE = base_settings.value("PORTS").toString();
  base_settings.endGroup();

  
  QSettings PortsSettings(PORTS_FILE, QSettings::IniFormat);
  PortsSettings.beginGroup("PORTS");
  auto keys = PortsSettings.allKeys();
  for(QString key: keys) { settings.emplace(key,   PortsSettings.value(key).toString()); }
  PortsSettings.endGroup();

  PortsSettings.beginGroup("PORTS_TEST");
  keys = PortsSettings.allKeys();
  for(QString key: keys) { settings.emplace(key,   PortsSettings.value(key).toString()); }
  PortsSettings.endGroup();

  PortsSettings::PrintSettingsList(); 

  qDebug() << TAG << "PORTS SETTINGS LOADED: " << file;
  return true;
}

void PortsSettings::LoadSettings()
{
  QString last_path;
  auto CheckPath = [&last_path](QString path) -> bool
  {
    QFile file(path);  

    last_path = path;
    if(path.isEmpty()) return false;
    if(!path.contains("TrainerSettings.ini")) return false;
    return file.exists(); 
  };
  QString env_Path = qgetenv("KLP_SETTINGS_PATH");

  std::vector<QString> settingsPathList;
  if(CheckPath(env_Path)) settingsPathList.push_back(last_path);
  if(CheckPath(QDir::currentPath() + "/DATA/TrainerData/TrainerSettings.ini")) settingsPathList.push_back(last_path);
  if(CheckPath(QString("/home/broms/DATA/TrainerData/TrainerSettings.ini"))) settingsPathList.push_back(last_path);
  if(CheckPath(QString("/home/broms/BACKUP/DATA/TrainerData/TrainerSettings.ini"))) settingsPathList.push_back(last_path);
  if(CheckPath(QString("/home/administrator/DATA/TrainerData/TrainerSettings.ini"))) settingsPathList.push_back(last_path);
  if(CheckPath(QString("/home/spp/DATA/TrainerData/TrainerSettings.ini"))) settingsPathList.push_back(last_path);
  
  QProcess findCommand;
  QStringList params; params << "/home" << "-maxdepth" << "4" << "-type" << "f" << "-name" << "TrainerSettings.ini";
  findCommand.start("/usr/bin/find",params); findCommand.waitForFinished(1000);
  QString result_str = findCommand.readAll(); QStringList result_list = result_str.split("\n");
  if(CheckPath(result_list[0])) settingsPathList.push_back(last_path);


  qDebug() << "=================================" << Qt::endl;
  for(auto& path: settingsPathList) qDebug() << "SETTINGS EXISTS: " << path;
  qDebug() << Qt::endl << "=================================";


  bool result = false;
  for(auto& path: settingsPathList)
  {
  result = TryLoadSettings(path);    if(result) return; qDebug() << TAG << "WARNING: LOAD" << path;
  }
  //===========================================
}
