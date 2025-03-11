#include <dbus/dbus.h>
#include <iostream>

#include "settings.h"

void send_message(DBusConnection *connection, const MyMessage &msg) {
  DBusMessage *message;
  DBusPendingCall *pending;
  DBusMessageIter iter;

  // Создаем сообщение
  message = dbus_message_new_method_call("com.example.MyService", "/com/example/MyObject",
                                         "com.example.MyInterface", "SendMyMessage");

  if (!message) {
    std::cerr << "Failed to create DBus message" << std::endl;
    return;
  }

  // Инициализируем итератор для добавления аргументов
  dbus_message_iter_init_append(message, &iter);

  msg.write_dbus(iter);

  // Отправляем сообщение и ждем ответа
  dbus_connection_send_with_reply(connection, message, &pending, -1);

  if (!pending) {
    std::cerr << "Failed to send DBus message" << std::endl;
    dbus_message_unref(message);
    return;
  }

  dbus_connection_flush(connection);
  dbus_message_unref(message);

  // Блокируем выполнение до получения ответа
  dbus_pending_call_block(pending);

  // Получаем ответ
  DBusMessage *reply = dbus_pending_call_steal_reply(pending);
  if (!reply) {
    std::cerr << "Failed to get DBus reply" << std::endl;
    dbus_pending_call_unref(pending);
    return;
  }

  // Извлекаем ответ от сервера
  DBusError error;
  dbus_error_init(&error);
  const char *response;

  if (!dbus_message_get_args(reply, &error, DBUS_TYPE_STRING, &response, DBUS_TYPE_INVALID)) {
    std::cerr << "Failed to parse reply: " << error.message << std::endl;
    dbus_error_free(&error);
  } else {
    std::cout << "Server response: " << response << std::endl;
  }

  // Освобождаем ресурсы
  dbus_message_unref(reply);
  dbus_pending_call_unref(pending);
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

  // Создаем и отправляем сообщение
  MyMessage msg;

  msg.value1 = 10;
  msg.value2 = 20;
  msg.value3 = 3.5;
  msg.value4 = "testStringMessage";
  msg.value5.value1 = 42;
  msg.value5.value2 = 43;

  send_message(connection, msg);

  dbus_connection_unref(connection);
  return 0;
}