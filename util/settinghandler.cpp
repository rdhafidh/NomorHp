#include "settinghandler.h"

SettingHandler::SettingHandler() {}

bool SettingHandler::writeDefaultSettings() {
  QScopedPointer<QSettings, QScopedPointerDeleteLater> settings(
      new QSettings("pengaturan.ini", QSettings::IniFormat));

  settings->setIniCodec("UTF-8");
  settings->setPath(QSettings::IniFormat, QSettings::UserScope, ".");
  settings->setDefaultFormat(QSettings::IniFormat);

  settings->beginGroup("table_paging");
  settings->setValue("ditampilkan", 10);
  settings->endGroup();


  settings->sync();
  return settings->isWritable();
}

bool SettingHandler::setSetting(const QString &group, const QString &key,
                                const QVariant &value) {
  QScopedPointer<QSettings, QScopedPointerDeleteLater> settings(
      new QSettings("pengaturan.ini", QSettings::IniFormat));

  settings->setIniCodec("UTF-8");
  settings->setPath(QSettings::IniFormat, QSettings::UserScope, ".");
  settings->setDefaultFormat(QSettings::IniFormat);
  settings->beginGroup(group);
  settings->setValue(key, value);
  settings->endGroup();
  settings->sync();
  return settings->isWritable();
}

bool SettingHandler::setSetting(const QString &fname, const QString &group,
                                const QString &key, const QVariant &value) {
  QScopedPointer<QSettings, QScopedPointerDeleteLater> settings(
      new QSettings(fname, QSettings::IniFormat));

  settings->setIniCodec("UTF-8");
  settings->setPath(QSettings::IniFormat, QSettings::UserScope, ".");
  settings->setDefaultFormat(QSettings::IniFormat);
  settings->beginGroup(group);
  settings->setValue(key, value);
  settings->endGroup();
  settings->sync();
  return settings->isWritable();
}

QVariant SettingHandler::getSetting(const QString &group, const QString &key) {
  QScopedPointer<QSettings, QScopedPointerDeleteLater> settings(
      new QSettings("pengaturan.ini", QSettings::IniFormat));
  settings->sync();
  settings->setIniCodec("UTF-8");
  settings->setPath(QSettings::IniFormat, QSettings::UserScope, ".");
  return settings->value(group + "/" + key, getDefaultValue(group, key));
}
QVariant SettingHandler::getDefaultValue(const QString &group,
                                         const QString &name) {
  if (group == "table_paging" && name == "ditampilkan") return 10;
  
  return "";
}
