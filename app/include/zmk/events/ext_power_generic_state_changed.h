/*
 * Copyright (c) 2020 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <zephyr.h>
#include <zmk/event_manager.h>

struct zmk_ext_power_generic_state_changed {
    int state;
};

ZMK_EVENT_DECLARE(zmk_ext_power_generic_state_changed);