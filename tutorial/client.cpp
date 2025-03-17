#include <iostream>
#include <memory>
#include <stdexcept>

#include "dBusConnection.h"

class DBusMessage {
public:
  DBusMessage(DBusConnection &conn, const char *destination, const char *path,
              const char *interface, const char *method)
      : msg(dbus_message_new_method_call(destination, path, interface, method)) {
    if (not msg) {
      throw std::runtime_error("Failed to create D-Bus message.");
    }
  }

  ~DBusMessage() {
    if (msg) {
      dbus_message_unref(msg);
    }
  }

  DBusMessage(const DBusMessage &) = delete;
  DBusMessage &operator=(const DBusMessage &) = delete;

  DBusMessage(DBusMessage &&other) noexcept : msg(other.msg) {
    other.msg = nullptr;
  }

  DBusMessage &operator=(DBusMessage &&other) noexcept {
    if (this != &other) {
      if (msg) {
        dbus_message_unref(msg);
      }
      msg = other.msg;
      other.msg = nullptr;
    }
    return *this;
  }

  void append_args(int a, int b) {
    if (not dbus_message_append_args(msg, DBUS_TYPE_INT32, &a, DBUS_TYPE_INT32, &b,
                                     DBUS_TYPE_INVALID)) {
      throw std::runtime_error("Failed to append arguments to D-Bus message.");
    }
  }

  DBusMessage *call(DBusConnection &conn, int timeout = -1) {
    DBusError err;
    dbus_error_init(&err);
    DBusMessage *reply = dbus_connection_send_with_reply_and_block(conn, msg, timeout, &err);
    if (dbus_error_is_set(&err)) {
      dbus_error_free(&err);
      throw std::runtime_error("Failed to send D-Bus message.");
    }
    return reply;
  }

  int get_return_value(DBusMessage *reply) {
    DBusError err;
    dbus_error_init(&err);
    int result;
    if (not dbus_message_get_args(reply, &err, DBUS_TYPE_INT32, &result, DBUS_TYPE_INVALID)) {
      dbus_error_free(&err);
      throw std::runtime_error("Failed to get return value from D-Bus message.");
    }
    return result;
  }

private:
  DBusMessage *msg = nullptr;
};

int main() {
  try {
    DBusConnection conn;
    DBusMessage msg(conn, "com.example.Calculator", "/com/example/CalculatorObject",
                    "com.example.CalculatorInterface", "Add");
    msg.append_args(2, 3);

    std::unique_ptr<DBusMessage, decltype(&dbus_message_unref)> reply(msg.call(conn),
                                                                      dbus_message_unref);
    int result = msg.get_return_value(reply.get());

    std::cout << "Result: " << result << std::endl;
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}