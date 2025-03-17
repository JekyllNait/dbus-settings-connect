#include "settings.h"

json MyMessage::json_schema = R"(
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "type": "object",
  "definitions": {
    "integer_parameter": {
      "type": "integer"
    },
    "float_parameter": {
      "type": "number"
    },
    "string_parameter": {
      "type": "string"
    }
  },
  "properties": {
    "value1": {"$ref": "#/definitions/integer_parameter"},
    "value2": {"$ref": "#/definitions/integer_parameter"},
    "value3": {"$ref": "#/definitions/float_parameter"},
    "value4": {"$ref": "#/definitions/string_parameter"},
    "value5": {
      "type": "object",
      "properties": {
        "value1": {"$ref": "#/definitions/integer_parameter"},
        "value2": {"$ref": "#/definitions/float_parameter"}
      },
      "required": ["value1", "value2"]
    }
  },
  "required": ["value1", "value2", "value3", "value4", "value5"]
}
)"_json;

void ParameterPack::write_dbus(DBusMessageIter &iter) const {
  DBusMessageIter struct_iter;
  // Открываем контейнер STRUCT
  dbus_message_iter_open_container(&iter, DBUS_TYPE_STRUCT, nullptr, &struct_iter);

  write_dbus_elems(struct_iter);

  // Закрываем контейнер STRUCT
  dbus_message_iter_close_container(&iter, &struct_iter);
}

void ParameterPack::write_dbus_elems(DBusMessageIter &iter) const {
  for (const auto &field : mFields) {
    field.write(iter);
  }
}

void ParameterPack::read_dbus(DBusMessageIter &iter) {
  // Открываем контейнер STRUCT
  /// @todo обрабатывать ошибку
  if (dbus_message_iter_get_arg_type(&iter) != DBUS_TYPE_STRUCT) {
    return;
  }

  DBusMessageIter struct_iter;
  dbus_message_iter_recurse(&iter, &struct_iter);

  read_dbus_elems(struct_iter);
}

void ParameterPack::read_dbus_elems(DBusMessageIter &iter) {
  for (auto &field : mFields) {
    field.read(iter);
  }
}

template <class T> SettingsParameter<T>::SettingsParameter() {
  if constexpr (std::is_same_v<T, int>) {
    mDBusParameterType = DBUS_TYPE_INT32;
  } else if constexpr (std::is_same_v<T, double>) {
    mDBusParameterType = DBUS_TYPE_DOUBLE;
  } else if constexpr (std::is_same_v<T, std::string>) {
    mDBusParameterType = DBUS_TYPE_STRING;
  } else if constexpr (std::is_same_v<T, MessageHandlerBase::MessageType>) {
    mDBusParameterType = DBUS_TYPE_INT32;
  } else {
    /// @todo invalid type
    static_assert(false);
    mDBusParameterType = DBUS_TYPE_INT32;
  }
}

template <class T> void SettingsParameter<T>::write_dbus(DBusMessageIter &struct_iter) const {
  // Добавляем поле структуры
  if constexpr (std::is_same_v<T, std::string>) {
    const char *value_cstr = mValue.c_str();
    dbus_message_iter_append_basic(&struct_iter, mDBusParameterType, &value_cstr);
  } else {
    dbus_message_iter_append_basic(&struct_iter, mDBusParameterType, &mValue);
  }
}

template <class T> void SettingsParameter<T>::read_dbus(DBusMessageIter &struct_iter) {
  if constexpr (std::is_same_v<T, std::string>) {
    const char *value_cstr;
    // Преобразуем C-строку в std::string
    dbus_message_iter_get_basic(&struct_iter, &value_cstr);
    dbus_message_iter_next(&struct_iter);
    mValue = std::string(value_cstr);
  } else {
    dbus_message_iter_get_basic(&struct_iter, &mValue);
    dbus_message_iter_next(&struct_iter);
  }
}

template <class T> SettingsParameter<T>::Type SettingsParameter<T>::get() const {
  return mValue;
}

template <class T> void SettingsParameter<T>::set(const Type &newValue) {
  mValue = newValue;
}

template <class T> SettingsParameter<T>::Type &SettingsParameter<T>::get_mutable() {
  return mValue;
}

template <class T> SettingsParameter<T> &SettingsParameter<T>::operator=(const Type &newValue) {
  set(newValue);
  return *this;
}

template <class T> SettingsParameter<T>::operator Type() const {
  return get();
}

MyInnerMessage::MyInnerMessage() {
  register_field(value1);
  register_field(value2);
}

MyInnerMessage &MyInnerMessage::get_mutable() {
  return *this;
}

MyInnerMessage MyInnerMessage::get() const {
  return *this;
}

MyMessage::MyMessage() {
  register_field(value1);
  register_field(value2);
  register_field(value3);
  register_field(value4);
  register_field(value5);
}

MyMessage &MyMessage::get_mutable() {
  return *this;
}

MyMessage MyMessage::get() const {
  return *this;
}

MessageHandlerBase::MessageHandlerBase() {
  register_field(messageType);
}

template <class S> MessageHeader<S>::MessageHeader() {
  if constexpr (std::is_same_v<S, MyMessage>) {
    messageType = MessageType::OUTER;
  }
  if constexpr (std::is_same_v<S, MyInnerMessage>) {
    messageType = MessageType::INNER;
  }
}

template class MessageHeader<MyMessage>;
template class MessageHeader<MyInnerMessage>;

template class SettingsParameter<int>;
template class SettingsParameter<double>;
template class SettingsParameter<std::string>;
template class SettingsParameter<MessageHandlerBase::MessageType>;
