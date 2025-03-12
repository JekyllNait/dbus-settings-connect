#include "settings.h"

json MyMessage::json_schema = R"(
{
    "$schema": "http://json-schema.org/draft-07/schema#",
    "type": "object",
    "properties": {
        "value1": {"type": "integer"},
        "value2": {"type": "integer"},
        "value3": {"type": "number"},
        "value4": {"type": "string"},
        "value5": {
            "type": "object",
            "properties": {
                "value1": {"type": "integer"},
                "value2": {"type": "number"}
            },
            "required": ["value1", "value2"]
        }
    },
    "required": ["value1", "value2", "value3", "value4", "value5"]
}
)"_json;

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

void to_json(json &j, const MyInnerMessage &msg) {
  j = json{{"value1", msg.value1}, {"value2", msg.value2}};
}

void from_json(const json &j, MyInnerMessage &msg) {
  j.at("value1").get_to(msg.value1);
  j.at("value2").get_to(msg.value2);
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

void to_json(json &j, const MyMessage &msg) {
  j = json{{"value1", msg.value1},
           {"value2", msg.value2},
           {"value3", msg.value3},
           {"value4", msg.value4},
           {"value5", msg.value5}};
}

void from_json(const json &j, MyMessage &msg) {
  j.at("value1").get_to(msg.value1);
  j.at("value2").get_to(msg.value2);
  j.at("value3").get_to(msg.value3);
  j.at("value4").get_to(msg.value4);
  j.at("value5").get_to(msg.value5);
}