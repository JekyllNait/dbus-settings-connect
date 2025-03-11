#include <dbus/dbus.h>
#include <iostream>

#include "settings.h"

DBusHandlerResult handle_message(DBusConnection *connection, DBusMessage *message,
                                 void *user_data) {
  if (dbus_message_is_method_call(message, "com.example.MyInterface", "SendMyMessage")) {
    DBusMessageIter iter;

    MyMessage mm;

    // Инициализируем итератор для чтения аргументов
    if (!dbus_message_iter_init(message, &iter)) {
      std::cerr << "Failed to initialize message iterator" << std::endl;
      return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
    }

    mm.read_dbus(iter);

    // Выводим полученные значения
    std::cout << "Received message: "
              << "value1 = " << mm.value1 << ", "
              << "value2 = " << mm.value2 << ", "
              << "value3 = " << mm.value3 << ", "
              << "value4 = " << mm.value4 << ", "
              << "value5.value1 = " << mm.value5.value1 << ", "
              << "value5.value2 = " << mm.value5.value2 << std::endl;

    // Отправляем ответ
    DBusMessage *reply = dbus_message_new_method_return(message);
    const char *response = "Message received!";
    dbus_message_append_args(reply, DBUS_TYPE_STRING, &response, DBUS_TYPE_INVALID);
    dbus_connection_send(connection, reply, nullptr);
    dbus_message_unref(reply);

    return DBUS_HANDLER_RESULT_HANDLED;
  }
  return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
}

int main() {
  DBusConnection *connection;
  DBusError error;

  dbus_error_init(&error);
  connection = dbus_bus_get(DBUS_BUS_SESSION, &error);

  if (dbus_error_is_set(&error)) {
    std::cerr << "DBus connection error: " << error.message << std::endl;
    dbus_error_free(&error);
    return 1;
  }

  dbus_bus_request_name(connection, "com.example.MyService", DBUS_NAME_FLAG_REPLACE_EXISTING,
                        &error);

  if (dbus_error_is_set(&error)) {
    std::cerr << "DBus name request error: " << error.message << std::endl;
    dbus_error_free(&error);
    return 1;
  }

  dbus_connection_add_filter(connection, handle_message, nullptr, nullptr);

  std::cout << "Server is running..." << std::endl;

  while (true) {
    dbus_connection_read_write_dispatch(connection, 1000);
  }

  dbus_connection_unref(connection);
  return 0;
}