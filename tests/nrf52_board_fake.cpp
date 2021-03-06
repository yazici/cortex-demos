/*******************************************************************************
    Copyright 2018 Google LLC

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        https://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*******************************************************************************/

#include "pinctrl.hpp"

#include "nrf52/pinctrl.hpp"

namespace pinctrl {

struct entry pinconfig_entries[] = {
    {
        .function = function::UARTE0_RXD,
        .pin = 13,
    },
    /*
     * Leave this unconfigured, to make sure
     * that it will be disconnected.
    {
        .function = function::UARTE0_CTS,
        .pin = 12,
    },
    */
    {
        .function = function::UARTE0_RTS,
        .pin = 14,
    },
    {
        .function = function::UARTE0_TXD,
        .pin = 3,
    },
    {
        .function = function::SAADC_CHAN0_POS,
        .pin = saadc::AIN3,
    },
    {
        .function = function::SAADC_CHAN2_POS,
        .pin = saadc::AIN5,
    },
};

struct config board_config = {
    .n_entries = sizeof(pinconfig_entries) / sizeof(pinconfig_entries[0]),
    .entries = pinconfig_entries,
};

struct config* board_pin_config = &board_config;

};
