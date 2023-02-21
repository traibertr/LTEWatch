/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// nRF Connect SDK / Zephyr
#include <zephyr.h>

// External
#include "platform-config.h"
#include "motordriverhal.h"
#include "factory.h"
#include "xf.h"

#include <lvgl.h>
#include <zephyr/drivers/display.h>
#include <stdio.h>
#include <string>

#include "config/disp-config.h"

#include <logging/log.h>
LOG_MODULE_REGISTER(main, LOG_LEVEL_DBG);

/* **************************************************************************************************************** */
/* Declaration section                                                                                              */
/* **************************************************************************************************************** */
// Global objects
static Factory factory;

int main(void)
{
    printk("\n=======================================================\n");
	printk("===== Starting LTEWatch App ... %s =====", CONFIG_BOARD);
    printk("\n=======================================================\n");

    // Initialize all gpios as disconnected inputs
    LOG_DBG("GpioHal init\n");
    hal::GpioHal::init();

    LOG_DBG("Lunch Factory\n");
    factory.launch(PROD_DEFAULT_MODE);

    LOG_DBG("Start XF");
    XF::start();

    //-----------------------------------------------------------------------------
}
