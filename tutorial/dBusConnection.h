#include <dbus/dbus.h>

class DBusConnection {
public:
  DBusConnection();
  ~DBusConnection();

  DBusConnection(const DBusConnection &) = delete;
  DBusConnection &operator=(const DBusConnection &) = delete;
  DBusConnection(DBusConnection &&other) noexcept;
  DBusConnection &operator=(DBusConnection &&other) noexcept;

  operator DBusConnection *() const;

private:
  DBusConnection *conn = nullptr;
};