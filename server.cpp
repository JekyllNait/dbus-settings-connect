#include <csignal>
#include <dbus/dbus.h>
#include <fstream>
#include <iostream>

#include "settings.h"
#include "settings.json.h"

static bool running = true;

template <class T> void processing(DBusMessageIter &iter) {
  T message;
  message.read_dbus(iter);

  json j = message;

  std::ofstream output("message.json");
  output << std::setw(4) << j << std::endl;
  output.close();
}

DBusHandlerResult handle_message(DBusConnection *connection, DBusMessage *message,
                                 void *user_data) {
  if (not dbus_message_is_method_call(message, "com.example.MyInterface", "SendMyMessage")) {
    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
  }

  DBusMessageIter iter;

  // Инициализируем итератор для чтения аргументов
  if (not dbus_message_iter_init(message, &iter)) {
    std::cerr << "Failed to initialize message iterator" << std::endl;
    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
  }

  MessageHandlerBase header;

  header.read_dbus(iter);

  MessageHandlerBase::MessageType type = header.messageType;

  dbus_message_iter_next(&iter);

  auto messageType = MessageHandlerBase::getMessageType(type);

  if (std::holds_alternative<MyInnerMessage>(messageType)) {
    std::cout << "MyInnerMessage\n";
    processing<MyInnerMessage>(iter);
  } else if (std::holds_alternative<MyMessage>(messageType)) {
    std::cout << "MyMessage\n";
    processing<MyMessage>(iter);
  }

  // Отправляем ответ
  DBusMessage *reply = dbus_message_new_method_return(message);

  if (not reply) {
    return DBUS_HANDLER_RESULT_NEED_MEMORY;
  }

  const char *response = "Message received!";
  if (not dbus_message_append_args(reply, DBUS_TYPE_STRING, &response, DBUS_TYPE_INVALID)) {
    dbus_message_unref(reply);
    return DBUS_HANDLER_RESULT_NEED_MEMORY;
  }

  dbus_connection_send(connection, reply, nullptr);
  dbus_message_unref(reply);

  return DBUS_HANDLER_RESULT_HANDLED;
}

void signal_handler(int signal) {
  running = false;
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

  signal(SIGINT, signal_handler);
  signal(SIGTERM, signal_handler);

  std::cout << "Server is running..." << std::endl;

  while (running) {
    dbus_connection_read_write_dispatch(connection, 1000);
  }

  dbus_connection_unref(connection);
  return 0;
}