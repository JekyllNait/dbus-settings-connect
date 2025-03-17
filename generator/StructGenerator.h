#pragma once

#include <iostream>
#include <nlohmann/json.hpp>
#include <sstream>
#include <string>

#include "StructModel.h"

using json = nlohmann::json;

class StructGenerator {
public:
  /// @brief Генерация C++ структур на основе JSON Schema.
  /// @param input JSON Schema, требующая C++ представления.
  /// @param output Выходной поток для записи сгенерированного кода.
  void generate(const json &input, std::stringstream &output) {
    StructModel rootStruct;
    rootStruct.mName = "MainStruct";

    if (input.contains("definitions")) {
      generateDefinitions(input["definitions"], rootStruct);
    }

    if (input.contains("properties")) {
      generateProperties(input["properties"], rootStruct);
    }

    output << "#pragma once\n\n";
    output << "#include <string>\n";
    output << "#include <vector>\n";
    output << "#include <tuple>\n";
    output << "#include <utility>\n\n";
    output << "#include \"settings.h\"\n\n";

    output << rootStruct.toString();
  }

private:
  /// @brief Генерация структур для определений (definitions) в JSON Schema.
  /// @param definitions JSON объект с определениями.
  /// @param resultStruct Структура, в которую добавляются сгенерированные определения.
  void generateDefinitions(const json &definitions, StructModel &resultStruct) {
    for (const auto &[key, value] : definitions.items()) {
      StructModel definitionStruct;
      definitionStruct.mName = capitalize(key);

      if (value.contains("properties")) {
        generateProperties(value["properties"], definitionStruct);
      }

      resultStruct.mNestedStructs.push_back(definitionStruct);
    }
  }

  /// @brief Генерация свойств (properties) для структуры.
  /// @param properties JSON объект со свойствами.
  /// @param resultStruct Структура, в которую добавляются сгенерированные свойства.
  void generateProperties(const json &properties, StructModel &resultStruct) {
    for (const auto &[key, value] : properties.items()) {
      StructModel::Field field;
      field.mName = key;
      field.mType = "SettingsParameter<" + getType(value, key) + ">";

      // Если это объект, создаем вложенную структуру
      if (value.contains("type") and value["type"] == "object") {
        StructModel nestedStruct;
        nestedStruct.mName = capitalize(key);
        generateProperties(value["properties"], nestedStruct);
        resultStruct.mNestedStructs.push_back(nestedStruct);
      }

      resultStruct.mFields.push_back(field);
    }
  }

  /// @brief Определение типа поля на основе JSON Schema.
  /// @param value JSON объект, описывающий поле.
  /// @param key Имя поля.
  /// @return Строка, представляющая тип поля в C++.
  std::string getType(const json &value, std::string const &key) {
    if (value.contains("$ref")) {
      std::string ref = value["$ref"];
      size_t pos = ref.find_last_of('/');
      if (pos != std::string::npos) {
        return capitalize(ref.substr(pos + 1));
      }
      return "unknown";
    } else if (value.contains("type")) {
      std::string type = value["type"];
      if (type == "integer") {
        return "int";
      } else if (type == "number") {
        return "double";
      } else if (type == "string") {
        return "std::string";
      } else if (type == "array" and value.contains("items")) {
        return "std::vector<" + getType(value["items"], key) + ">";
      } else if (type == "object") {
        return capitalize(key);
      }
    }
    return "unknown";
  }

  /// @brief Преобразует строку в CamelCase (например, "integer_parameter" -> "IntegerParameter").
  /// @param str Исходная строка.
  /// @return Строка в формате CamelCase.
  std::string capitalize(const std::string &str) {
    std::string result = str;
    result[0] = std::toupper(result[0]);
    for (size_t i = 1; i < result.length(); ++i) {
      if (result[i - 1] == '_') {
        result[i] = std::toupper(result[i]);
      }
    }
    result.erase(std::remove(result.begin(), result.end(), '_'), result.end());
    return result;
  }
};