/*
 * Copyright (c) 2020 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#define ZMK_KEYMAP_ACTIVATE_BEHAVIORS_NODE DT_INST(0, zmk_keymap_activate_behaviors)
#define ZMK_KEYMAP_HAS_ACTIVATE_BEHAVIORS DT_NODE_HAS_PROP(ZMK_KEYMAP_ACTIVATE_BEHAVIORS_NODE, activate_behaviors)
#define ZMK_KEYMAP_ACTIVATE_BEHAVIORS_LEN DT_PROP_LEN(ZMK_KEYMAP_ACTIVATE_BEHAVIORS_NODE, activate_behaviors)
#define ZMK_KEYMAP_ACTIVATE_BEHAVIORS_BY_IDX(idx) DT_PHANDLE_BY_IDX(ZMK_KEYMAP_ACTIVATE_BEHAVIORS_NODE, activate_behaviors, idx)

#define ZMK_KEYMAP_DEACTIVATE_BEHAVIORS_NODE DT_INST(0, zmk_keymap_deactivate_behaviors)
#define ZMK_KEYMAP_HAS_DEACTIVATE_BEHAVIORS DT_NODE_HAS_PROP(ZMK_KEYMAP_DEACTIVATE_BEHAVIORS_NODE, deactivate_behaviors)
#define ZMK_KEYMAP_DEACTIVATE_BEHAVIORS_LEN DT_PROP_LEN(ZMK_KEYMAP_DEACTIVATE_BEHAVIORS_NODE, deactivate_behaviors)
#define ZMK_KEYMAP_DEACTIVATE_BEHAVIORS_BY_IDX(idx) DT_PHANDLE_BY_IDX(ZMK_KEYMAP_DEACTIVATE_BEHAVIORS_NODE, deactivate_behaviors, idx)

typedef uint32_t zmk_keymap_layers_state_t;

uint8_t zmk_keymap_layer_default();
zmk_keymap_layers_state_t zmk_keymap_layer_state();
bool zmk_keymap_layer_active(uint8_t layer);
uint8_t zmk_keymap_highest_layer_active();
int zmk_keymap_layer_activate(uint8_t layer);
int zmk_keymap_layer_deactivate(uint8_t layer);
int zmk_keymap_layer_toggle(uint8_t layer);
int zmk_keymap_layer_to(uint8_t layer);
const char *zmk_keymap_layer_label(uint8_t layer);

int zmk_keymap_position_state_changed(uint32_t position, bool pressed, int64_t timestamp);
