#include "settings.h"

void MyInnerMessage::write_dbus(DBusMessageIter &iter) const {
  DBusMessageIter struct_iter;
  // Открываем контейнер STRUCT
  dbus_message_iter_open_container(&iter, DBUS_TYPE_STRUCT, nullptr, &struct_iter);

  // Добавляем поля структуры
  dbus_message_iter_append_basic(&struct_iter, DBUS_TYPE_INT32, &value1);
  dbus_message_iter_append_basic(&struct_iter, DBUS_TYPE_DOUBLE, &value2);

  // Закрываем контейнер STRUCT
  dbus_message_iter_close_container(&iter, &struct_iter);
}

void MyInnerMessage::read_dbus(DBusMessageIter &iter) {
  // Открываем контейнер STRUCT
  /// @todo обрабатывать ошибку
  if (dbus_message_iter_get_arg_type(&iter) != DBUS_TYPE_STRUCT) {
    return;
  }

  DBusMessageIter struct_iter;
  dbus_message_iter_recurse(&iter, &struct_iter);

  // Извлекаем поля структуры
  dbus_message_iter_get_basic(&struct_iter, &value1);
  dbus_message_iter_next(&struct_iter);
  dbus_message_iter_get_basic(&struct_iter, &value2);
  dbus_message_iter_next(&struct_iter);
}

void MyMessage::write_dbus(DBusMessageIter &iter) const {
  DBusMessageIter struct_iter;
  // Открываем контейнер STRUCT
  dbus_message_iter_open_container(&iter, DBUS_TYPE_STRUCT, nullptr, &struct_iter);

  const char *value4_cstr = value4.c_str();

  // Добавляем поля структуры
  dbus_message_iter_append_basic(&struct_iter, DBUS_TYPE_INT32, &value1);
  dbus_message_iter_append_basic(&struct_iter, DBUS_TYPE_INT32, &value2);
  dbus_message_iter_append_basic(&struct_iter, DBUS_TYPE_DOUBLE, &value3);
  dbus_message_iter_append_basic(&struct_iter, DBUS_TYPE_STRING, &value4_cstr);

  value5.write_dbus(struct_iter);

  // Закрываем контейнер STRUCT
  dbus_message_iter_close_container(&iter, &struct_iter);
}

void MyMessage::read_dbus(DBusMessageIter &iter) {
  // Открываем контейнер STRUCT
  /// @todo обрабатывать ошибку
  if (dbus_message_iter_get_arg_type(&iter) != DBUS_TYPE_STRUCT) {
    return;
  }

  DBusMessageIter struct_iter;
  dbus_message_iter_recurse(&iter, &struct_iter);

  const char *value4_cstr;

  // Извлекаем поля структуры
  dbus_message_iter_get_basic(&struct_iter, &value1);
  dbus_message_iter_next(&struct_iter);
  dbus_message_iter_get_basic(&struct_iter, &value2);
  dbus_message_iter_next(&struct_iter);
  dbus_message_iter_get_basic(&struct_iter, &value3);
  dbus_message_iter_next(&struct_iter);
  dbus_message_iter_get_basic(&struct_iter, &value4_cstr);
  dbus_message_iter_next(&struct_iter);

  // Преобразуем C-строку в std::string
  value4 = std::string(value4_cstr);

  value5.read_dbus(struct_iter);
}
