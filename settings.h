#pragma once

#include <string>

#include <dbus/dbus.h>
#include <nlohmann/json-schema.hpp>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class Parameter {
public:
  virtual void write_dbus(DBusMessageIter &iter) const = 0;
  virtual void read_dbus(DBusMessageIter &iter) = 0;
};

class ParameterPack {
public:
  virtual void write_dbus(DBusMessageIter &iter) const = 0;
  virtual void read_dbus(DBusMessageIter &iter) = 0;
};

template <class T> class SettingsParameter : public Parameter {
public:
  using Type = T;

  SettingsParameter();

  void write_dbus(DBusMessageIter &iter) const override;
  void read_dbus(DBusMessageIter &iter) override;

  Type get() const;
  void set(const Type &newValue);
  Type &get_mutable();

  SettingsParameter<T> &operator=(const Type &newValue);
  operator Type() const;

protected:
  using DBusParameterType = int;

  DBusParameterType mDBusParameterType;
  Type mValue;
};

class MyInnerMessage : public ParameterPack {
public:
  SettingsParameter<int> value1;
  SettingsParameter<double> value2;

  void write_dbus(DBusMessageIter &iter) const;
  void read_dbus(DBusMessageIter &iter);

  MyInnerMessage &get_mutable();
  MyInnerMessage get() const;
};

class MyMessage : public ParameterPack {
public:
  SettingsParameter<int> value1;
  SettingsParameter<int> value2;
  SettingsParameter<double> value3;
  SettingsParameter<std::string> value4;
  MyInnerMessage value5;

  void write_dbus(DBusMessageIter &iter) const;
  void read_dbus(DBusMessageIter &iter);

  MyMessage &get_mutable();
  MyMessage get() const;

  static json json_schema;
};

void to_json(json &j, const MyInnerMessage &msg);
void from_json(const json &j, MyInnerMessage &msg);
void to_json(json &j, const MyMessage &msg);
void from_json(const json &j, MyMessage &msg);
