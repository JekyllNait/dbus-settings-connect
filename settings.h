#pragma once

#include <functional>
#include <string>
#include <vector>

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
  void write_dbus(DBusMessageIter &iter) const;
  virtual void write_dbus_elems(DBusMessageIter &iter) const;

  void read_dbus(DBusMessageIter &iter);
  virtual void read_dbus_elems(DBusMessageIter &iter);

protected:
  struct Field {
    std::function<void(DBusMessageIter &)> write;
    std::function<void(DBusMessageIter &)> read;
  };

  std::vector<Field> mFields;

  template <typename T> void register_field(T &field);
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
  MyInnerMessage();

public:
  SettingsParameter<int> value1;
  SettingsParameter<double> value2;

  MyInnerMessage &get_mutable();
  MyInnerMessage get() const;
};

class MyMessage : public ParameterPack {
public:
  MyMessage();

public:
  SettingsParameter<int> value1;
  SettingsParameter<int> value2;
  SettingsParameter<double> value3;
  SettingsParameter<std::string> value4;
  MyInnerMessage value5;

  MyMessage &get_mutable();
  MyMessage get() const;

  static json json_schema;
};

void to_json(json &j, const MyInnerMessage &msg);
void from_json(const json &j, MyInnerMessage &msg);
void to_json(json &j, const MyMessage &msg);
void from_json(const json &j, MyMessage &msg);

template <typename T> void ParameterPack::register_field(T &field) {
  mFields.push_back({[&field](DBusMessageIter &iter) { field.write_dbus(iter); },
                     [&field](DBusMessageIter &iter) { field.read_dbus(iter); }});
}