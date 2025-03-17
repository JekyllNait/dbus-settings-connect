#include <stdexcept>

#include "dBusConnection.h"

DBusConnection::DBusConnection() {
  DBusError err;
  dbus_error_init(&err);
  conn = dbus_bus_get(DBUS_BUS_SESSION, &err);
  if (dbus_error_is_set(&err)) {
    dbus_error_free(&err);
    throw std::runtime_error("Failed to connect to the D-Bus session bus.");
  }
}

DBusConnection::~DBusConnection() {
  if (conn) {
    dbus_connection_unref(conn);
  }
}

DBusConnection::DBusConnection(DBusConnection &&other) noexcept : conn(other.conn) {
  other.conn = nullptr;
}

DBusConnection &DBusConnection::operator=(DBusConnection &&other) noexcept {
  if (this != &other) {
    if (conn) {
      dbus_connection_unref(conn);
    }
    conn = other.conn;
    other.conn = nullptr;
  }
  return *this;
}

DBusConnection::operator DBusConnection *() const {
  return conn;
}
