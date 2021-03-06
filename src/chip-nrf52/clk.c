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

#include "clk.h"

#include <stdint.h>

#include "memio.h"
#include "nrf52/clk.h"

#define CLOCK_BASE      (0x40000000)

#define LFCLKSRC        (CLOCK_BASE + 0x518)
#define LFCLKSRC_SRC_MASK       (3)
#define LFCLKSRC_BYPASS       (1 << 16)
#define LFCLKSRC_EXTERNAL       (1 << 17)

#define LFCLKSTAT       (CLOCK_BASE + 0x418)
#define LFCLKSTAT_RUNNING     (1 << 16)

#define TASK(n)     (CLOCK_BASE + ((n) * 4))

enum {
    TASK_HFCLKSTART,
    TASK_HFCLKSTOP,
    TASK_LFCLKSTART,
    TASK_LFCLKSTOP,
    TASK_CAL,
    TASK_CTSTART,
    TASK_CTSTOP,
};

enum {
    LFCLK_SRC_RC,
    LFCLK_SRC_XTAL,
    LFCLK_SRC_SYNTH,

    LFCLK_SRC_LAST,
};

static void start_lfclk_sync(void) {
    raw_write32(TASK(TASK_LFCLKSTART), 1);
    while (!(raw_read32(LFCLKSTAT) & LFCLKSTAT_RUNNING));
}

int clk_request(int clock_id) {
    int ret = -1;
    switch (clock_id) {
        case NRF52_LFCLK_RC:
            raw_write32(LFCLKSRC, LFCLK_SRC_RC);
            start_lfclk_sync();
            ret = 0;
            break;
        case NRF52_LFCLK_XTAL:
            raw_write32(LFCLKSRC, LFCLK_SRC_XTAL);
            start_lfclk_sync();
            ret = 0;
            break;
    }

    return ret;
}
