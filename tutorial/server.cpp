#include <dbus/dbus.h>
#include <iostream>
#include <stdexcept>
#include <memory>

#include "dBusConnection.h"

class DBusService {
public:
    DBusService(DBusConnection& conn, const char* service_name)
        : conn(conn) {
        dbus_bus_request_name(conn, service_name, DBUS_NAME_FLAG_REPLACE_EXISTING, nullptr);
    }

    void register_method(const char* interface, const char* method, DBusHandleMessageFunction handler) {
        DBusObjectPathVTable vtable = {};
        vtable.message_function = handler;
        dbus_connection_register_object_path(conn, "/com/example/CalculatorObject", &vtable, nullptr);
    }

private:
    DBusConnection& conn;
};

DBusHandlerResult handle_add_method(DBusConnection* conn, DBusMessage* msg, void* user_data) {
    DBusError err;
    dbus_error_init(&err);

    int a, b;
    if (!dbus_message_get_args(msg, &err, DBUS_TYPE_INT32, &a, DBUS_TYPE_INT32, &b, DBUS_TYPE_INVALID)) {
        dbus_error_free(&err);
        return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
    }

    int sum = a + b;
    DBusMessage* reply = dbus_message_new_method_return(msg);
    dbus_message_append_args(reply, DBUS_TYPE_INT32, &sum, DBUS_TYPE_INVALID);
    dbus_connection_send(conn, reply, nullptr);
    dbus_message_unref(reply);

    return DBUS_HANDLER_RESULT_HANDLED;
}

int main() {
    try {
        DBusConnection conn;
        DBusService service(conn, "com.example.Calculator");
        service.register_method("com.example.CalculatorInterface", "Add", handle_add_method);

        while (true) {
            dbus_connection_read_write_dispatch(conn, 1000);
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}