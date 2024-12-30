#include "dbus.h"
#include "dbus-print-message.h"

namespace esphome {
namespace dbus {

static const char *const TAG = "dbus-wrapper";

std::string dbusIterToString(DBusMessageIter *iter) {
  std::string ret = "";
  if (dbus_message_iter_get_arg_type(iter) == DBUS_TYPE_STRING) {
    char *value = NULL;
    dbus_message_iter_get_basic(iter, &value);
    return value;
  } else if (dbus_message_iter_get_arg_type(iter) == DBUS_TYPE_VARIANT) {
    DBusMessageIter variter;
    dbus_message_iter_recurse(iter, &variter);
    return dbusIterToString(&variter);
  } else if (dbus_message_iter_get_arg_type(iter) == DBUS_TYPE_ARRAY) {
    DBusMessageIter arrayiter;
    dbus_message_iter_recurse(iter, &arrayiter);
    while (dbus_message_iter_get_arg_type(&arrayiter) != DBUS_TYPE_INVALID) {
      ret += dbusIterToString(&arrayiter);
      dbus_message_iter_next(&arrayiter);
    }

  } else {
    printf("unimplemented type\n");
    print_iter(iter, false);
  }

  return ret;
}

DBusWrapper::~DBusWrapper() {
#warning free conn;
}

void DBusWrapper::setup() {
  printf("======================================== DBusWrapper::setup()\n");
  //  ESP_LOGV(TAG, "%s  DBusWrapper::setup(): '%s'", prefix, (obj)->unique_id().c_str());
  //  this->publish_state("DBusTextSensor::setup()");

  DBusError err;
  // initialiset the errors
  dbus_error_init(&err);
  // connect to the system bus and check for errors
  // this->conn = dbus_bus_get(DBUS_BUS_SESSION, &err);
  // always creates a new connection.
  this->conn = dbus_bus_get_private(DBUS_BUS_SESSION, &err);
  if (dbus_error_is_set(&err)) {
    fprintf(stderr, "Connection Error (%s)\n", err.message);
    dbus_error_free(&err);
  }
  if (NULL == this->conn) {
    exit(1);
  }

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

std::string DBusWrapper::send(const std::string &dbus_destination, const std::string &dbus_path,
                              const std::string &dbus_interface, const std::string &dbus_method,
                              const std::list<std::string> &dbus_args, const std::list<std::string> &properties,
                              const std::string &property_separator) {
  printf("DBus::send destination:%s, path:%s, interface.method:%s.%s, args:[%d]\n", dbus_destination.c_str(),
         dbus_path.c_str(), dbus_interface.c_str(), dbus_method.c_str(), dbus_args.size());
  if (this->conn == NULL) {
    printf("conn==NULL\n");
    return "conn==NULL";
  }
  DBusMessage *msg;
  DBusMessageIter args;
  DBusError err;
  DBusPendingCall *pending;
  dbus_uint32_t level;
  bool stat;

  // initialiset the errors
  dbus_error_init(&err);
  //  this->publish_state("Update");

  /*
      // request our name on the bus
      int ret = dbus_bus_request_name(conn, "test.method.caller", DBUS_NAME_FLAG_REPLACE_EXISTING , &err);
      if (dbus_error_is_set(&err)) {
          fprintf(stderr, "Name Error (%s)\n", err.message);
          dbus_error_free(&err);
      }
      if (DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER != ret) {
         exit(1);
      }
     */

  // create a new method call and check for errors
  msg = dbus_message_new_method_call(
      dbus_destination.c_str(),  // destination
      dbus_path.c_str(),         // path
      dbus_interface.c_str(),    // interface -> bei dbus-send ist interface+'.'+method zusammengehängt
      dbus_method.c_str());      // method name "Get"
  if (NULL == msg) {
    fprintf(stderr, "Message Null\n");
    exit(1);
  }

  // append arguments
  // append arguments as list:
  // if (!dbus_message_append_args(msg, DBUS_TYPE_STRING, &interface, DBUS_TYPE_STRING, &param, DBUS_TYPE_INVALID)) {
  DBusMessageIter iter_arg;
  dbus_message_iter_init_append(msg, &iter_arg);
  for (auto it = dbus_args.begin(); it != dbus_args.end(); ++it) {
    const char *param = it->c_str();
    printf("------ add arg: [%s]\n", param);
    if (!dbus_message_iter_append_basic(&iter_arg, DBUS_TYPE_STRING, &param)) {
      fprintf(stderr, "Out Of Memory!\n");
      exit(1);
    }
  }

  // send message and get a handle for a reply
  if (!dbus_connection_send_with_reply(conn, msg, &pending, -1)) {  // -1 is default timeout
    fprintf(stderr, "Out Of Memory!\n");
    exit(1);
  }
  if (NULL == pending) {
    fprintf(stderr, "Pending Call Null\n");
    exit(1);
  }
  dbus_connection_flush(conn);

  printf("Request Sent\n");

  // free message
  dbus_message_unref(msg);

  // block until we recieve a reply
  dbus_pending_call_block(pending);

  // get the reply message
  msg = dbus_pending_call_steal_reply(pending);
  // free the pending message handle
  dbus_pending_call_unref(pending);
  if (NULL == msg) {
    fprintf(stderr, "Reply Null\n");
    exit(1);
  }

  // print_message(msg, false);

  // read the parameters
  std::string ret = "";

  std::list<std::string>::iterator it;

  for (auto const &it : properties) {
    if (ret.length() > 0) {
      ret += property_separator;
    }
    ret += this->getProperty(msg, it);
  }

  // free reply
  dbus_message_unref(msg);

  // this->publish_state("dbus sync data!");
  return ret;
}

std::string DBusWrapper::getProperty(DBusMessage *msg, const std::string &searchKey) {
  printf("DBusTextSensor::getProperty(msg=%p, searchKey='%s')\n", msg, searchKey.c_str());
  DBusMessageIter iter, iter_variant;
  char *value_str_ptr;
  std::string ret;

  if (!dbus_message_iter_init(msg, &iter)) {
    fprintf(stderr, "Message has no arguments!\n");
    dbus_message_unref(msg);
    return "Message has no arguments";
  }
  if (dbus_message_iter_get_arg_type(&iter) == DBUS_TYPE_VARIANT) {
    printf("get_property dbus_message_iter_get_arg_type[0] is variant\n");
    dbus_message_iter_recurse(&iter, &iter_variant);
    int type = dbus_message_iter_get_arg_type(&iter_variant);
    switch (type) {
      case DBUS_TYPE_STRING:
        dbus_message_iter_get_basic(&iter_variant, &value_str_ptr);
        // strncpy(property->value_string, value_str_ptr, property->value_string_len);
        printf("get_property : %s\n", value_str_ptr);
        return value_str_ptr;
        break;
      case DBUS_TYPE_INT16:
      case DBUS_TYPE_UINT16:
      case DBUS_TYPE_INT32:
      case DBUS_TYPE_UINT32:
      case DBUS_TYPE_INT64:
      case DBUS_TYPE_UINT64: {
        dbus_int64_t i = 0;
        dbus_message_iter_get_basic(&iter_variant, &i);
#ifdef DBUS_INT64_PRINTF_MODIFIER
        printf("get_property : %" DBUS_INT64_PRINTF_MODIFIER "i\n", i);
#else
        printf("get_property : %i\n", (int) i);
#endif
        return std::to_string(i);
        break;
      }
      case DBUS_TYPE_BOOLEAN: {
        dbus_bool_t val;
        dbus_message_iter_get_basic(&iter_variant, &val);
        printf("boolean %s\n", val ? "true" : "false");
        return val ? "true" : "false";
        break;
      }
      case DBUS_TYPE_STRUCT:
        printf("get_property dbus_message_iter_get_arg_type[1] is struct [TODO]\n");
        break;
      case DBUS_TYPE_DICT_ENTRY:
        printf("get_property dbus_message_iter_get_arg_type[1] is dict entry [TODO]\n");
        break;
      case DBUS_TYPE_ARRAY:
        printf("get_property dbus_message_iter_get_arg_type[1] is array\n");
        DBusMessageIter array_iter;
        dbus_message_iter_recurse(&iter_variant, &array_iter);
        while (dbus_message_iter_get_arg_type(&array_iter) != DBUS_TYPE_INVALID) {
          if (dbus_message_iter_get_arg_type(&array_iter) == DBUS_TYPE_DICT_ENTRY) {
            printf("DBUS_TYPE_DICT_ENTRY\n");
            DBusMessageIter subiter;
            dbus_message_iter_recurse(&array_iter, &subiter);
            printf("dict entry(\n");
            bool add = false;
            char *key = NULL;
            if (dbus_message_iter_get_arg_type(&subiter) == DBUS_TYPE_STRING) {
              dbus_message_iter_get_basic(&subiter, &key);
              printf("key: >>>%s<<<\n", key);
              if (searchKey == key) {
                add = true;
              }
            } else {
              printf("---- KEY is not a string!!!\n");
              print_iter(&subiter, false);
            }
            dbus_message_iter_next(&subiter);
            if (add) {
              std::string value = dbusIterToString(&subiter);
              printf("add      value=%s\n", value.c_str());
              return value;
            }
          } else if (dbus_message_iter_get_arg_type(&array_iter) == DBUS_TYPE_STRUCT) {
            printf("DBUS_TYPE_STRUCT [TODO]\n");
          } else if (dbus_message_iter_get_arg_type(&array_iter) == DBUS_TYPE_ARRAY) {
            printf("DBUS_TYPE_ARRAY [TODO]\n");
          } else {
            printf("something else\n");
            print_iter(&array_iter, false);
          }
          dbus_message_iter_next(&array_iter);
        }
        printf("get_property dbus_message_iter_get_arg_type is array done\n");
        break;
      default:
        printf("get_property dbus_message_iter_get_arg_type error\n");
        return "error";
        break;
    }
  } else if (dbus_message_iter_get_arg_type(&iter) == DBUS_TYPE_STRING) {
    printf("get_property dbus_message_iter_get_arg_type[0] is string\n");
    dbus_message_iter_get_basic(&iter, &value_str_ptr);
    // strncpy(property->value_string, value_str_ptr, property->value_string_len);
    printf("get_property : %s\n", value_str_ptr);
    return value_str_ptr;
  } else if (dbus_message_iter_get_arg_type(&iter) == DBUS_TYPE_BOOLEAN) {
    printf("get_property dbus_message_iter_get_arg_type[0] is boolean\n");
    dbus_bool_t val;
    dbus_message_iter_get_basic(&iter, &val);
    printf("boolean %s\n", val ? "true" : "false");
    return val ? "true" : "false";
  } else if (dbus_message_iter_get_arg_type(&iter) == DBUS_TYPE_STRUCT) {
    printf("get_property dbus_message_iter_get_arg_type[0] is struct\n");
    return "struct";
  } else {
    printf("get_property dbus_message_iter_get_arg_type not variant error\n");
    return "error";
  }
  return "tag not found";
}

// https://github.com/freedesktop/dbus/blob/master/dbus/dbus-bus.c
void DBusWrapper::registerForSignal(const std::string &dbus_properties, const std::string &dbus_path) {
  printf("DBusWrapper::registerForSignal(properties=%s, path=%s)\n", dbus_properties.c_str(), dbus_path.c_str());
  DBusError err;
  dbus_error_init(&err);

  // add a rule for which messages we want to see
  /*
   * "type='signal',sender='org.freedesktop.DBus',
   * interface='org.freedesktop.DBus',member='Foo',
   * path='/bar/foo',destination=':452345.34'"
   */
  dbus_bus_add_match(conn, ("type='signal',interface='" + dbus_properties + "',path='" + dbus_path + "'").c_str(),
                     &err);  // see signals from the given interface
  dbus_connection_flush(conn);
  if (dbus_error_is_set(&err)) {
    fprintf(stderr, "Match Error (%s)\n", err.message);
    exit(1);
  }
  printf("Match rule sent\n");
}

#warning todo
/*
void DBus::loop() {
    // spam printf("DBusTextSensor::loop\n");

  dbus_connection_read_write(conn, 0);
    DBusMessage* msg = dbus_connection_pop_message(conn);
  char* sigvalue=NULL;

      // loop again if we haven't read a message
      if (NULL == msg) {
         // printf("no dbus signal\n");
         return;
      }
    printf("Got Signal with value %s\n", sigvalue);
    print_message(msg, true);
    dbus_message_unref(msg);
//	  printf("DBus::loop update\n");
//	  this->update(); read message???
    printf("DBus::loop done\n");
}
*/

}  // namespace dbus
}  // namespace esphome
