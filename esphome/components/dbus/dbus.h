#pragma once

#include <string>
#include <list>
#include <dbus/dbus.h>
#include <dbus/dbus-glib.h>

#include "esphome/core/defines.h"
// #include "esphome/core/entity_base.h"
#include "esphome/core/component.h"
//#include "esphome/components/text_sensor/text_sensor.h"
#include "esphome/core/automation.h"

#include "dbus-wrapper.h"

namespace esphome {
namespace dbus {

// class DBus : public EntityBase {
class DBus : public Component, public DBusWrapper {
 public:
  ~DBus();
  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override;
  //  std::string unique_id() override;

  void loop() override;
  //  void update() override; -> pollingComponent

  // Actions

  void send(const optional<std::string> &dbus_destination, const optional<std::string> &dbus_path,
            const optional<std::string> &dbus_interface, const optional<std::string> &dbus_method,
            const optional<std::list<std::string> > &dbus_args);

 protected:
};

}  // namespace dbus
}  // namespace esphome
