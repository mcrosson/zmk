/*
 * Copyright (c) 2020 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#define DT_DRV_COMPAT zmk_behavior_ext_power

#include <device.h>
#include <devicetree.h>
#include <drivers/behavior.h>
#include <drivers/ext_power.h>

#include <dt-bindings/zmk/ext_power.h>

#include <logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

static int on_keymap_binding_pressed(struct zmk_behavior_binding *binding,
                                     struct zmk_behavior_binding_event event) {
    const struct device *ext_power = device_get_binding("EXT_POWER");
    if (ext_power == NULL) {
        LOG_ERR("Unable to retrieve ext_power device: %d", binding->param1);
        return -EIO;
    }

    switch (binding->param1) {
    case EXT_POWER_OFF_CMD:
        return ext_power_disable(ext_power, true);
    case EXT_POWER_ON_CMD:
        return ext_power_enable(ext_power, true);
    case EXT_POWER_TOGGLE_CMD:
        if (ext_power_get(ext_power) > 0)
            return ext_power_disable(ext_power, true);
        else
            return ext_power_enable(ext_power, true);
    default:
        LOG_ERR("Unknown ext_power command: %d", binding->param1);
    }

    return -ENOTSUP;
}

static int on_keymap_binding_released(struct zmk_behavior_binding *binding,
                                      struct zmk_behavior_binding_event event) {
    return ZMK_BEHAVIOR_OPAQUE;
}

static int behavior_ext_power_init(const struct device *dev) { return 0; };

static const struct behavior_driver_api behavior_ext_power_driver_api = {
    .binding_pressed = on_keymap_binding_pressed,
    .binding_released = on_keymap_binding_released,
};

DEVICE_AND_API_INIT(behavior_ext_power, DT_INST_LABEL(0), behavior_ext_power_init, NULL, NULL,
                    APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY, &behavior_ext_power_driver_api);
