/*
 * Copyright (c) 2020 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#define DT_DRV_COMPAT zmk_behavior_toggle_key

#include <device.h>
#include <drivers/behavior.h>
#include <logging/log.h>
#include <zmk/behavior.h>

#include <zmk/matrix.h>
#include <zmk/endpoints.h>
#include <zmk/event_manager.h>
#include <zmk/events/position_state_changed.h>
#include <zmk/events/keycode_state_changed.h>
#include <zmk/events/modifiers_state_changed.h>
#include <zmk/hid.h>
#include <zmk/keymap.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#if DT_HAS_COMPAT_STATUS_OKAY(DT_DRV_COMPAT)

#define ZMK_BHV_TOGGLE_KEY_MAX_HELD 10

#define ZMK_BHV_TOGGLE_KEY_POSITION_FREE ULONG_MAX

struct behavior_toggle_key_config {
    struct zmk_behavior_binding behavior;
};

struct active_toggle_key {
    uint32_t position;
    uint32_t param1;
    uint32_t param2;
    const struct behavior_toggle_key_config *config;
    // usage page and keycode for the key that is being modified by this toggle key
    uint8_t modified_key_usage_page;
    uint32_t modified_key_keycode;
};

struct active_toggle_key active_toggle_keys[ZMK_BHV_TOGGLE_KEY_MAX_HELD] = {};

static struct active_toggle_key *store_toggle_key(uint32_t position, uint32_t param1,
                                                  uint32_t param2,
                                                  const struct behavior_toggle_key_config *config) {
    for (int i = 0; i < ZMK_BHV_TOGGLE_KEY_MAX_HELD; i++) {
        struct active_toggle_key *const toggle_key = &active_toggle_keys[i];
        if (toggle_key->position != ZMK_BHV_TOGGLE_KEY_POSITION_FREE) {
            continue;
        }
        toggle_key->position = position;
        toggle_key->param1 = param1;
        toggle_key->param2 = param2;
        toggle_key->config = config;
        toggle_key->modified_key_usage_page = 0;
        toggle_key->modified_key_keycode = 0;
        return toggle_key;
    }
    return NULL;
}

static void clear_toggle_key(struct active_toggle_key *toggle_key) {
    LOG_DBG("clear_toggle_key");
    toggle_key->position = ZMK_BHV_TOGGLE_KEY_POSITION_FREE;
}

static struct active_toggle_key *find_toggle_key(uint32_t position) {
    for (int i = 0; i < ZMK_BHV_TOGGLE_KEY_MAX_HELD; i++) {
        if (active_toggle_keys[i].position == position) {
            return &active_toggle_keys[i];
        }
    }
    return NULL;
}

static inline int press_toggle_key_behavior(struct active_toggle_key *toggle_key,
                                            int64_t timestamp) {
    LOG_DBG("press_toggle_key_behavior");
    struct zmk_behavior_binding binding = {
        .behavior_dev = toggle_key->config->behavior.behavior_dev,
        .param1 = toggle_key->param1,
        .param2 = toggle_key->param2,
    };
    struct zmk_behavior_binding_event event = {
        .position = toggle_key->position,
        .timestamp = timestamp,
    };
    return behavior_keymap_binding_pressed(&binding, event);
}

static inline int release_toggle_key_behavior(struct active_toggle_key *toggle_key,
                                              int64_t timestamp) {
    LOG_DBG("release_toggle_key_behavior");
    struct zmk_behavior_binding binding = {
        .behavior_dev = toggle_key->config->behavior.behavior_dev,
        .param1 = toggle_key->param1,
        .param2 = toggle_key->param2,
    };
    struct zmk_behavior_binding_event event = {
        .position = toggle_key->position,
        .timestamp = timestamp,
    };

    clear_toggle_key(toggle_key);
    return behavior_keymap_binding_released(&binding, event);
}

static int on_toggle_key_binding_pressed(struct zmk_behavior_binding *binding,
                                         struct zmk_behavior_binding_event event) {
    LOG_DBG("on_toggle_key_binding_pressed");
    const struct device *dev = device_get_binding(binding->behavior_dev);
    const struct behavior_toggle_key_config *cfg = dev->config;
    struct active_toggle_key *toggle_key;
    toggle_key = find_toggle_key(event.position);
    if (toggle_key != NULL) {
        release_toggle_key_behavior(toggle_key, event.timestamp);
    }
    else {
        toggle_key = store_toggle_key(event.position, binding->param1, binding->param2, cfg);
        if (toggle_key == NULL) {
            LOG_ERR("unable to store toggle key, did you press more than %d toggle_key?",
                    ZMK_BHV_TOGGLE_KEY_MAX_HELD);
            return ZMK_BEHAVIOR_OPAQUE;
        }

        press_toggle_key_behavior(toggle_key, event.timestamp);
        LOG_DBG("%d new toggle_key", event.position);
    }
    return ZMK_BEHAVIOR_OPAQUE;
}

static int on_toggle_key_binding_released(struct zmk_behavior_binding *binding,
                                          struct zmk_behavior_binding_event event) {
    LOG_DBG("on_toggle_key_binding_released");
    return ZMK_BEHAVIOR_OPAQUE;
}

static const struct behavior_driver_api behavior_toggle_key_driver_api = {
    .binding_pressed = on_toggle_key_binding_pressed,
    .binding_released = on_toggle_key_binding_released,
};

ZMK_SUBSCRIPTION(behavior_toggle_key, zmk_keycode_state_changed);

static int behavior_toggle_key_init(const struct device *dev) {
    LOG_DBG("behavior_toggle_key_init");
    static bool init_first_run = true;
    if (init_first_run) {
        for (int i = 0; i < ZMK_BHV_TOGGLE_KEY_MAX_HELD; i++) {
            active_toggle_keys[i].position = ZMK_BHV_TOGGLE_KEY_POSITION_FREE;
        }
    }
    init_first_run = false;
    return 0;
}

struct behavior_toggle_key_data {};
static struct behavior_toggle_key_data behavior_toggle_key_data;

#define KP_INST(n)                                                                                 \
    static struct behavior_toggle_key_config behavior_toggle_key_config_##n = {                    \
        .behavior = ZMK_KEYMAP_EXTRACT_BINDING(0, DT_DRV_INST(n)),                                 \
    };                                                                                             \
    DEVICE_AND_API_INIT(behavior_toggle_key_##n, DT_INST_LABEL(n), behavior_togogle_key_init,       \
                        &behavior_toggle_key_data, &behavior_toggle_key_config_##n, APPLICATION,   \
                        CONFIG_KERNEL_INIT_PRIORITY_DEFAULT, &behavior_toggle_key_driver_api);

DT_INST_FOREACH_STATUS_OKAY(KP_INST)

#endif
