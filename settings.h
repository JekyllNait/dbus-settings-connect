#pragma once

#include <string>

#include <dbus/dbus.h>
#include <nlohmann/json-schema.hpp>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct MyInnerMessage {
  int value1;
  double value2;

  void write_dbus(DBusMessageIter &iter) const;
  void read_dbus(DBusMessageIter &iter);
};

struct MyMessage {
  int value1;
  int value2;
  double value3;
  std::string value4;
  MyInnerMessage value5;

  void write_dbus(DBusMessageIter &iter) const;
  void read_dbus(DBusMessageIter &iter);

  static json json_schema;
};

void to_json(json &j, const MyInnerMessage &msg);
void from_json(const json &j, MyInnerMessage &msg);
void to_json(json &j, const MyMessage &msg);
void from_json(const json &j, MyMessage &msg);