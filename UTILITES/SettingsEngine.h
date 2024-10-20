#ifndef SETTINGS_H
#define SETTINGS_H

#include <QProcess>
#include <QDebug>

class SettingsPath
{
    public:
    static std::map<QString,QString> settings;

    static void    LoadSettings();
    static bool TryLoadSettings(QString file);

    static void PrintSettingsList() 
    {
      for(auto& record: settings) qDebug() << "[ SETTINGS ] " << "LOAD: " << std::get<0>(record) << std::get<1>(record); 
    }

    static QString& GetPath(const QString& setting_name)
    {
        if(!settings.empty()) return settings[setting_name];

        TryLoadSettings("/home/spp/DATA/TrainerData/TrainerSettings.ini");
        return settings[setting_name];
    };
};

class PortsSettings
{
    public:
    PortsSettings();
    static std::map<QString,QString> settings;
    static void LoadSettings();
    static bool TryLoadSettings(QString file);
    static QString GetSettings(const QString& setting_name)
    {
        if(!settings.empty()) return settings[setting_name];

        LoadSettings();
        return settings[setting_name];
    };
    static void PrintSettingsList() 
    {
      qDebug()  << "[ SETTINGS ]" << "SETTINGS LIST";
      for(auto& record: settings) 
        qDebug() << "[ SETTINGS ] " << "[ PORTS NAME:]" << std::get<0>(record) << std::get<1>(record); 
    }
};

extern PortsSettings PortsSettingsStorage;



#endif //SETTINGS_H
