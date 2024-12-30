# zypper install dbus-1-devel glib2-devel dbus-1-glib-devel

from esphome.const import (
    CONF_ID,
    PLATFORM_HOST,
)

from esphome import automation

# from esphome.automation import LambdaAction
import esphome.codegen as cg
import esphome.config_validation as cv


# from esphome.core import CORE, Lambda
from esphome.core import CORE
from esphome.coroutine import coroutine_with_priority

# from esphome.cpp_helpers import setup_entity

CODEOWNERS = ["@esphome/core"]

CONF_DBUS_DESTINATION = "dbus_destination"
CONF_DBUS_PATH = "dbus_path"
CONF_DBUS_INTERFACE = "dbus_interface"
CONF_DBUS_METHOD = "dbus_method"
CONF_DBUS_ARGS = "dbus_args"


dbus_ns = cg.esphome_ns.namespace("dbus")
DBus = dbus_ns.class_("DBus", cg.Component)

DBusSendAction = dbus_ns.class_(
    "DBusSendAction", automation.Action, cg.Parented.template(DBus)
)


# Actions
@automation.register_action(
    "dbus.send",
    DBusSendAction,
    # cv.maybe_simple_value(
    cv.Schema(
        {
            cv.GenerateID(): cv.use_id(DBus),
            # cv.GenerateID(): cv.declare_id(DBus), # new ID ????
            cv.Required(CONF_DBUS_DESTINATION): cv.templatable(cv.string),
            cv.Required(CONF_DBUS_PATH): cv.templatable(cv.string),
            cv.Required(CONF_DBUS_INTERFACE): cv.templatable(cv.string),
            cv.Required(CONF_DBUS_METHOD): cv.templatable(cv.string),
            cv.Optional(CONF_DBUS_ARGS, default=False): cv.ensure_list(cv.string),
        },
    ),
)
async def dbus_send_action(config, action_id, template_arg, args):
    print("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! dbus_send_action")
    # als lambda func:
    # args_ = [
    #  str(config[CONF_DBUS_DESTINATION]),
    #  str(config[CONF_DBUS_PATH]),
    #  str(config[CONF_DBUS_INTERFACE]),
    #  str(config[CONF_DBUS_PROPERTIES]),
    #  str(config[CONF_DBUS_ARGS]),
    # ]

    # text = str(cg.statement(DBusSendAction(*args_)))
    # lambda_ = await cg.process_lambda(Lambda(text), args, return_type=cg.void)
    # var = cg.new_Pvariable(action_id, template_arg, lambda_)

    # esphome/components/ezo_pmp/
    # paren = await cg.get_variable(config[CONF_ID])
    # return cg.new_Pvariable(action_id, template_arg, paren)

    # esphome/components/media_player/
    var = cg.new_Pvariable(action_id, template_arg)
    await cg.register_parented(var, config[CONF_ID])
    dbus_destination = await cg.templatable(
        config[CONF_DBUS_DESTINATION], args, cg.std_string
    )
    cg.add(var.set_dbus_destination(dbus_destination))
    dbus_path = await cg.templatable(config[CONF_DBUS_PATH], args, cg.std_string)
    cg.add(var.set_dbus_path(dbus_path))
    dbus_interface = await cg.templatable(
        config[CONF_DBUS_INTERFACE], args, cg.std_string
    )
    cg.add(var.set_dbus_interface(dbus_interface))
    dbus_method = await cg.templatable(config[CONF_DBUS_METHOD], args, cg.std_string)
    cg.add(var.set_dbus_method(dbus_method))
    dbus_args = await cg.templatable(config[CONF_DBUS_ARGS], args, cg.std_string)
    cg.add(var.set_dbus_args(dbus_args))
    return var


CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(DBus),
        }
    ).extend(cv.COMPONENT_SCHEMA)
    #    .extend(cv.polling_component_schema("60s"))
    #    .extend(
    #        {
    #            cv.GenerateID(): cv.declare_id(DBusTextSensor),
    #            cv.Optional(CONF_DBUS_DESTINATION, default=False): cv.string,
    #            cv.Optional(CONF_DBUS_PATH, default=False): cv.string,
    #            cv.Optional(CONF_DBUS_INTERFACE, default=False): cv.string,
    #            cv.Optional(CONF_DBUS_PROPERTIES, default=False): cv.string,
    #            cv.Optional(CONF_DBUS_ARGS, default=False): cv.string,
    #        }
    #    )
    #    .extend(cv.COMPONENT_SCHEMA)
)


@coroutine_with_priority(100.0)
async def to_code(config):
    print("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! dbus to_code")
    if not CORE.is_host:
        return cv.only_on([PLATFORM_HOST])("DBUS")

    cg.add_global(dbus_ns.using)
    cg.add_define("USE_DBUS")

    var = cg.new_Pvariable(config[CONF_ID])
    # var = await cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    #    cg.add(var.set_dbus_service(config[CONF_DBUS_DESTINATION]))
    #    cg.add(var.set_dbus_path(config[CONF_DBUS_PATH]))
    #    cg.add(var.set_dbus_interface(config[CONF_DBUS_INTERFACE]))
    #    cg.add(var.set_dbus_properties(config[CONF_DBUS_PROPERTIES]))
    #    cg.add(var.set_dbus_args(config[CONF_DBUS_ARGS]))
    # FIXME: $$(pkg-config --cflags dbus-1) $$(pkg-config --cflags glib-2.0)
    cg.add_build_flag(
        "-I/usr/include/dbus-1.0 -I/usr/lib64/dbus-1.0/include -I/usr/include/glib-2.0 -I/usr/lib64/glib-2.0/include"
    )
    # pkg-config --libs dbus-1
    cg.add_build_flag("-ldbus-1 -g")
