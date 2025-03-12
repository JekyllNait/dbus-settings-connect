project('dbus-test', 'cpp',
  version: '1.0',
  default_options: ['cpp_std=c++20'])

# Исходные файлы
server_sources = files([
  'server.cpp',
  'settings.cpp'
])

client_sources = files([
  'client.cpp',
  'settings.cpp'
])

dbus_dep = dependency('dbus-1')
nlohmann_json = dependency('nlohmann_json')
nlohmann_json_schema_validator_dep = dependency('nlohmann_json_schema_validator', method: 'cmake')

# Исполняемые файлы
executable('server', server_sources, dependencies: [dbus_dep, nlohmann_json, nlohmann_json_schema_validator_dep])
executable('client', client_sources, dependencies: [dbus_dep, nlohmann_json, nlohmann_json_schema_validator_dep])
