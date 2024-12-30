#include "dbus.h"
#include "esphome/core/log.h"
#include "esphome/core/application.h"

#include "dbus-print-message.h"

// dbus-send --session --print-reply --dest=org.mpris.MediaPlayer2.amarok /org/mpris/MediaPlayer2
// org.freedesktop.DBus.Properties.Get string:'org.mpris.MediaPlayer2.Player' string:'Metadata'
//                                                                                                ^^^interface ^^
//                                                                                                ^^method
// https://stackoverflow.com/questions/47095118/c-gdbus-how-to-fetch-the-property-of-interface-using-gdbus-library
// #define DBUS_SERVICE      "org.mpris.MediaPlayer2.amarok"
// #define DBUS_PATH         "/org/mpris/MediaPlayer2"
// // #define DBUS_INTERFACE    "org.freedesktop.MediaPlayer2"
// #define DBUS_INTERFACE    "org.mpris.MediaPlayer2.Player"
// #define DBUS_PROPERTIES   "org.freedesktop.DBus.Properties"
//-- #define DBUS_woswasi    "org.mpris.MediaPlayer2.Player"
//-- #define DBUS_args       "Metadata"

namespace esphome {
namespace dbus {

static const char *const TAG = "dbus";

/*
std::string dbusIterToString(DBusMessageIter *iter)
{
  std::string ret="";
  if(dbus_message_iter_get_arg_type(iter) == DBUS_TYPE_STRING) {
    char *value=NULL;
    dbus_message_iter_get_basic(iter, &value);
    return value;
  } else if (dbus_message_iter_get_arg_type(iter) == DBUS_TYPE_VARIANT) {
    DBusMessageIter variter;
    dbus_message_iter_recurse (iter, &variter);
    return dbusIterToString(&variter);
  } else if (dbus_message_iter_get_arg_type(iter) == DBUS_TYPE_ARRAY) {
    DBusMessageIter arrayiter;
    dbus_message_iter_recurse (iter, &arrayiter);
    while ( dbus_message_iter_get_arg_type (&arrayiter) != DBUS_TYPE_INVALID) {
      ret += dbusIterToString(&arrayiter);
      dbus_message_iter_next (&arrayiter);
    }

  } else {
    printf("unimplemented type\n");
    print_iter(iter, false);
  }

  return ret;
}
*/

DBus::~DBus() {
#warning free conn;
}
void DBus::setup() {
  printf("[%s] DBus::setup()\n", "DBUS");
  ESP_LOGV(TAG, "%s  DBus::setup(): '%s'", prefix, (obj)->unique_id().c_str());
  //  this->publish_state("DBusTextSensor::setup()");
  this->DBusWrapper::setup();

  printf("[%s] DBus::setup() conn=%p\n", "DBUS", this->conn);
  /*
    DBusError err;
    // initialiset the errors
    dbus_error_init(&err);
    // connect to the system bus and check for errors
    this->conn = dbus_bus_get(DBUS_BUS_SESSION, &err);
    if (dbus_error_is_set(&err)) {
      fprintf(stderr, "Connection Error (%s)\n", err.message);
      dbus_error_free(&err);
    }
    if (NULL == this->conn) {
      exit(1);
    }
  */
#warning TODO
  //  this->update();
  //  this->registerForSignal();
}

// https://github.com/wware/stuff/blob/master/dbus-example/dbus-example.c
/*
void DBus::update() {
printf("======================================== DBus::update()\n");
  ESP_LOGV(TAG, "%s  DBus::update(): '%s'", prefix, (obj)->unique_id().c_str());
}
*/

void DBus::send(const optional<std::string> &dbus_destination, const optional<std::string> &dbus_path,
                const optional<std::string> &dbus_interface, const optional<std::string> &dbus_method,
                const optional<std::list<std::string> > &dbus_args) {
  if (!dbus_destination.has_value()) {
    printf("no dbus_destination\n");
    return;
  }
  if (!dbus_path.has_value()) {
    printf("no dbus_path\n");
    return;
  }
  if (!dbus_interface.has_value()) {
    printf("no dbus_interface\n");
    return;
  }
  if (!dbus_method.has_value()) {
    printf("no dbus_method\n");
    return;
  }
  if (!dbus_args.has_value()) {
    printf("no dbus_args\n");
    return;
  }
  this->DBusWrapper::send(dbus_destination.value(), dbus_path.value(), dbus_interface.value(), dbus_method.value(),
                          dbus_args.value());
}

float DBus::get_setup_priority() const { return setup_priority::DATA; }
// void DBusTextSensor::set_hide_timestamp(bool hide_timestamp) { this->hide_timestamp_ = hide_timestamp; }
// std::string DBus::unique_id() { return get_mac_address() + "-dbus"; }
void DBus::dump_config() {
  ESP_LOGCONFIG(TAG, "dbus", this);
#warning todo
}

void DBus::loop() {
  // spam printf("DBusTextSensor::loop\n");
#warning todo

  dbus_connection_read_write(conn, 0);
  DBusMessage *msg = dbus_connection_pop_message(conn);

  // loop again if we haven't read a message
  if (NULL == msg) {
    // printf("no dbus signal\n");
    return;
  }
  printf("[%s] DBus::loop() Got Signal\n", "DBUS");
  print_message(msg, true);
  dbus_message_unref(msg);
  //	  printf("DBus::loop update\n");
  //	  this->update(); read message???
  printf("[%s] DBus::loop() done\n", "DBUS");
}

}  // namespace dbus
}  // namespace esphome
