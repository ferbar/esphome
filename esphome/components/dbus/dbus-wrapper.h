#pragma once

// #include <string.h>
#include <dbus/dbus.h>
#include <dbus/dbus-glib.h>

#include "esphome/core/defines.h"

namespace esphome {
namespace dbus {

std::string dbusIterToString(DBusMessageIter *iter);

// class DBus : public EntityBase {
class DBusWrapper {
 public:
  ~DBusWrapper();
  void setup();

  //  void loop() override;

  std::string send(const std::string &dbus_destination, const std::string &dbus_path, const std::string &dbus_interface,
                   const std::string &dbus_method, const std::list<std::string> &dbus_args,
                   const std::list<std::string> &properties = {}, const std::string &property_separator = "");

  std::string getProperty(DBusMessage *msg, const std::string &searchKey);

  /*
    // für die Action
    void send(const optional<std::string> &dbus_destination, const optional<std::string> &dbus_path,
      const optional<std::string> &dbus_interface,
      const optional<std::string> &dbus_method,
    const optional<std::string> &dbus_properties,
    const optional<std::string> &dbus_args);
  */

 protected:
  DBusConnection *conn{NULL};
  void registerForSignal(const std::string &dbus_properties, const std::string &dbus_path);
};

}  // namespace dbus
}  // namespace esphome
