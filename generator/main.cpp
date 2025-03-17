#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include <nlohmann/json.hpp>

#include "StructGenerator.h"

using json = nlohmann::json;

/// @brief Загружает JSON Schema из файла.
/// @param path Путь к файлу с JSON Schema.
/// @return Загруженный JSON объект.
json getSchema(const std::filesystem::path &path) {
  std::ifstream input_file(path);
  json schema;
  input_file >> schema;
  return schema;
}

/// @brief Записывает сгенерированный код в файл.
/// @param content Содержимое, которое нужно записать в файл.
void writeStruct(const std::string &content) {
  std::ofstream output_file("output_file.h");
  output_file << content;
  output_file.close();
  std::cout << "Файл output_file.h успешно сгенерирован!" << std::endl;
}

/// @brief Основная функция программы.
/// @return Код завершения программы.
int main() {
  json input = getSchema("input.json");

  StructGenerator generator;
  std::stringstream ss;
  generator.generate(input, ss);

  writeStruct(ss.str());

  return 0;
}