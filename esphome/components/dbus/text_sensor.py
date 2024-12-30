# zypper install dbus-1-devel glib2-devel dbus-1-glib-devel

# from esphome import automation
import esphome.codegen as cg
import esphome.config_validation as cv

# FIXME: das is bei allen anderen text_sensor.py auch drinnen, nur da gehts nicht
# pylint: disable = no-name-in-module
from esphome.components import text_sensor
from esphome.const import (
    ENTITY_CATEGORY_DIAGNOSTIC,
    ICON_NEW_BOX,
)


from . import (
    CONF_DBUS_DESTINATION,
    CONF_DBUS_PATH,
    CONF_DBUS_INTERFACE,
    CONF_DBUS_METHOD,
    CONF_DBUS_ARGS,
)

# IS_PLATFORM_COMPONENT = True -> nur light switch text etc...
DEPENDENCIES = ["dbus"]


CONF_PROPERTIES = "properties"
CONF_PROPERTY_SEPARATOR = "property_separator"

dbus_ns = cg.esphome_ns.namespace("dbus")
# DBus = dbus_ns.class_("DBus", cg.Component)


DBusTextSensor = dbus_ns.class_("DBusTextSensor", text_sensor.TextSensor, cg.Component)

# version/text_sensor.py
CONFIG_SCHEMA = (
    text_sensor.text_sensor_schema(
        icon=ICON_NEW_BOX,
        entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
    )
    .extend(
        {
            cv.GenerateID(): cv.declare_id(DBusTextSensor),
            # Hint: default=False geht nicht für string !!!! FIXME
            cv.Optional(CONF_DBUS_DESTINATION, default=False): cv.string,
            cv.Optional(CONF_DBUS_PATH, default=False): cv.string,
            cv.Optional(CONF_DBUS_INTERFACE, default=False): cv.string,
            cv.Optional(CONF_DBUS_METHOD, default="Get"): cv.string,
            # FIXME: wenn nur string dann in automatisch in liste konvertieren
            cv.Optional(CONF_DBUS_ARGS, default=False): cv.ensure_list(cv.string),
            # FIXME: das kamma auch mit simple irgendwie auch machen
            cv.Required(CONF_PROPERTIES): cv.ensure_list(cv.string),
            cv.Optional(CONF_PROPERTY_SEPARATOR, default=", "): cv.string,
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
    .extend(cv.polling_component_schema("60s"))
)


async def to_code(config):
    print("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! dbus to_code")
    var = await text_sensor.new_text_sensor(config)
    await cg.register_component(var, config)
    cg.add(var.set_dbus_destination(config[CONF_DBUS_DESTINATION]))
    cg.add(var.set_dbus_path(config[CONF_DBUS_PATH]))
    cg.add(var.set_dbus_interface(config[CONF_DBUS_INTERFACE]))
    cg.add(var.set_dbus_method(config[CONF_DBUS_METHOD]))
    cg.add(var.set_dbus_args(config[CONF_DBUS_ARGS]))
    cg.add(var.set_properties(config[CONF_PROPERTIES]))
    cg.add(var.set_property_separator(config[CONF_PROPERTY_SEPARATOR]))
