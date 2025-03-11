#pragma once

#include <dbus/dbus.h>

#include <string>

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
};