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

#pragma once

#include "memio.h"
#include "driver/peripheral.hpp"

namespace nrf52 {

class Peripheral : virtual public driver::Peripheral {
    public:
        Peripheral() {};
        void trigger_task(int task) {
            raw_write32(base_ + task * 4, 1);
        }

        void busy_wait_and_clear_event(int evt) {
            while (!raw_read32(base_ + kEventsOffset + evt * 4));
            clear_event(evt);
        }

    private:
        void clear_event(int evt) override {
            raw_write32(base_ + kEventsOffset + evt * 4, 0);
        }

        bool is_event_active(int evt) override {
            return raw_read32(base_ + kEventsOffset + evt * 4);
        }

        static constexpr auto kEventsOffset = 0x100;
};

}  // namespace nrf52
