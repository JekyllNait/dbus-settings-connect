#pragma once

#include <map>
#include <sstream>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

class StructModel {
public:
  /// @brief Класс, представляющий поле структуры.
  class Field {
  public:
    std::string mType;
    std::string mName;

    /// @brief Преобразует поле в строку для генерации кода.
    /// @param indentLevel Уровень отступа для форматирования.
    /// @return Строка, представляющая поле в C++.
    std::string toString(int indentLevel = 1) const {
      return std::string(indentLevel * 2, ' ') + mType + " " + mName + ";\n";
    }
  };

  std::string mName;
  std::vector<Field> mFields;
  std::vector<StructModel> mNestedStructs;

  /// @brief Преобразует всю структуру в строку для генерации кода.
  /// @return Строка, представляющая структуру в C++.
  std::string toString() const {
    std::stringstream ss;
    _to_string(ss, 0);
    return ss.str();
  }

private:
  /// @brief Рекурсивно преобразует структуру в строку для генерации кода.
  /// @param result Поток для записи результата.
  /// @param indentLevel Уровень отступа для форматирования.
  void _to_string(std::stringstream &result, int indentLevel) const {
    result << std::string(indentLevel * 2, ' ') << "struct " << mName
           << " : public ParameterPack {\n";

    // Генерация вложенных структур
    for (const auto &nestedStruct : mNestedStructs) {
      nestedStruct._to_string(result, indentLevel + 1);
    }

    // Генерация полей
    for (const auto &field : mFields) {
      result << field.toString(indentLevel + 1);
    }

    // Генерация методов и статического поля
    result << std::string((indentLevel + 1) * 2, ' ')
           << "static constexpr auto json_fields = std::make_tuple(\n";
    for (size_t i = 0; i < mFields.size(); ++i) {
      result << std::string((indentLevel + 2) * 2, ' ') << "std::make_pair(\"" << mFields[i].mName
             << "\", &" << mName << "::" << mFields[i].mName << ")";
      if (i < mFields.size() - 1) {
        result << ",\n";
      } else {
        result << "\n";
      }
    }
    result << std::string((indentLevel + 1) * 2, ' ') << ");\n\n";

    result << std::string((indentLevel + 1) * 2, ' ') << mName << " &get_mutable() {\n";
    result << std::string((indentLevel + 2) * 2, ' ') << "return *this;\n";
    result << std::string((indentLevel + 1) * 2, ' ') << "}\n\n";

    result << std::string((indentLevel + 1) * 2, ' ') << mName << " get() const {\n";
    result << std::string((indentLevel + 2) * 2, ' ') << "return *this;\n";
    result << std::string((indentLevel + 1) * 2, ' ') << "}\n\n";

    result << std::string((indentLevel + 1) * 2, ' ') << mName << "() {\n";
    for (const auto &field : mFields) {
      result << std::string((indentLevel + 2) * 2, ' ') << "register_field(" << field.mName
             << ");\n";
    }
    result << std::string((indentLevel + 1) * 2, ' ') << "}\n";

    result << std::string(indentLevel * 2, ' ') << "};\n\n";
  }
};