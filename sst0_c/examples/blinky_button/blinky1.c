/*============================================================================
* Super-Simple Tasker (SST/C) Example
*
* Copyright (C) 2006-2023 Quantum Leaps, <state-machine.com>.
*
* SPDX-License-Identifier: MIT
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the "Software"),
* to deal in the Software without restriction, including without limitation
* the rights to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
* DEALINGS IN THE SOFTWARE.
============================================================================*/
#include "sst.h"           /* SST framework */
#include "bsp.h"           /* Board Support Package interface */
#include "blinky_button.h" /* application shared interface */

DBC_MODULE_NAME("blinky1")

/*..........................................................................*/
typedef struct {    /* Blinky1 task */
    SST_Task super; /* inherit SST_Task */
    uint16_t toggles;
    uint8_t ticks;
    uint8_t tick_ctr;
} Blinky1;

static void Blinky1_init(Blinky1 * const me, SST_Evt const * const ie);
static void Blinky1_dispatch(Blinky1 * const me, SST_Evt const * const e);

/*..........................................................................*/
static Blinky1 Blinky1_inst; /* the Blinky instance */
SST_Task * const AO_Blinky1 = &Blinky1_inst.super; /* opaque AO pointer */

/*..........................................................................*/
void Blinky1_ctor(void) {
    Blinky1 * const me = &Blinky1_inst;
    SST_Task_ctor(
       &me->super,
       (SST_Handler)&Blinky1_init,
       (SST_Handler)&Blinky1_dispatch);
}

/*..........................................................................*/
static void Blinky1_init(Blinky1 * const me, SST_Evt const * const ie) {
    /* the initial event must be provided and must be WORKLOAD_SIG */
    DBC_REQUIRE(300,
        (ie != (SST_Evt const *)0) && (ie->sig == BLINKY_WORK_SIG));

    me->toggles = SST_EVT_DOWNCAST(BlinkyWorkEvt, ie)->toggles;
    me->ticks = SST_EVT_DOWNCAST(BlinkyWorkEvt, ie)->ticks;
    me->tick_ctr = me->ticks;
}
/*..........................................................................*/
static void Blinky1_dispatch(Blinky1 * const me, SST_Evt const * const e) {
    switch (e->sig) {
        case TICK_SIG: {
            --me->tick_ctr;
            if (me->tick_ctr == 0U) {
                me->tick_ctr = me->ticks;
                for (uint16_t i = me->toggles; i > 0U; --i) {
                    BSP_d5on();
                    BSP_d5off();
                }
            }
            break;
        }
        case BLINKY_WORK_SIG: {
            BSP_d5on();
            me->toggles = SST_EVT_DOWNCAST(BlinkyWorkEvt, e)->toggles;
            me->ticks = SST_EVT_DOWNCAST(BlinkyWorkEvt, e)->ticks;
            BSP_d5off();
            break;
        }
        default: {
            DBC_ERROR(500); /* unexpected event */
            break;
        }
    }
}
