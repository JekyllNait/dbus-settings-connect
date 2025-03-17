#include <type_traits>

template <typename T, typename... Args>
void serialize_fields(json &j, const T &obj, const std::tuple<Args...> &fields) {
  std::apply([&](const auto &...field) { ((j[field.first] = (obj.*field.second).get()), ...); },
             fields);
}

template <typename T, typename... Args>
void deserialize_fields(const json &j, T &obj, const std::tuple<Args...> &fields) {
  std::apply(
      [&](const auto &...field) {
        ((j.at(field.first).get_to((obj.*field.second).get_mutable()), ...));
      },
      fields);
}

template <typename T> void to_json(json &j, const T &obj) {
  serialize_fields(j, obj, T::json_fields);
}

template <typename T> void from_json(const json &j, T &obj) {
  deserialize_fields(j, obj, T::json_fields);
}