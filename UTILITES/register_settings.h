#ifndef SETTINGS_H
#define SETTINGS_H

#include <QProcess>
#include <QDebug>

class SettingsRegister
{
    public:
    static std::map<QString,QString>            settings;
    static std::map<QString,float>                settings_values;
    static std::map<QString,std::pair<float,float>> settings_pairs;
    static bool FLAG_SETTINGS_LOADED;
    static std::pair<float,float> CAMERA_IMAGE_SIZE;

    static void    LoadSettings();
    static bool TryLoadSettings(QString file, QString GROUP);

    static void AppendSettings(QString key, QString setting);
    static void AppendSettings(QString key, float setting);
    static void AppendSettings(QString key, std::pair<float,float> setting);

    static void ResetSettings(QString key, QString setting);
    static void ResetSettings(QString key, float setting);
    static void ResetSettings(QString key, std::pair<float,float> setting);

    static void PrintSettingsList() 
    {
      for(auto& record: settings) qDebug() << "[ SETTINGS ] " << "LOAD: " << std::get<0>(record) << std::get<1>(record); 
      for(auto& record: settings_values) qDebug() << "[ SETTINGS ] " << "LOAD: " << std::get<0>(record) << std::get<1>(record); 
      for(auto& record: settings_pairs) qDebug() << "[ SETTINGS ] " << "LOAD: " << std::get<0>(record) << std::get<1>(record); 
    }

    static void PrintSetting(QString key) 
    {
      if(settings.contains(key))        { qDebug() << "[ SETTINGS ] " << key << settings[key];        return; }
      if(settings_values.contains(key)) { qDebug() << "[ SETTINGS ] " << key << settings_values[key]; return; }
      if(settings_pairs.contains(key))  { qDebug() << "[ SETTINGS ] " << key << settings_pairs[key];  return; }
    }

    static QString GetString(const QString& setting_name)
    {
        if(!FLAG_SETTINGS_LOADED) LoadSettings();

        if( settings.empty()) return "NONE";
        if(!settings.contains(setting_name)) return "NONE";

        return settings[setting_name];

    };

    static float GetValue(const QString& setting_name)
    {
        if(!FLAG_SETTINGS_LOADED) LoadSettings();

        if( settings_values.empty()) return 0;
        if(!settings_values.contains(setting_name)) return 0;

        return settings_values[setting_name];
    };

    static std::pair<float,float> GetPair(const QString& setting_name)
    {
        if(!FLAG_SETTINGS_LOADED) LoadSettings();

        if( settings_pairs.empty()) return std::pair<float,float>(0,0);
        if(!settings_pairs.contains(setting_name)) return std::pair<float,float>(0,0);

        return settings_pairs[setting_name];
    };
    private:
    static void SetStaticSettings();
};


#endif //SETTINGS_H
