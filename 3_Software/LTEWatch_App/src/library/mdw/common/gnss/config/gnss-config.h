/* Copyright (C) MSE Lausanne Hes-so VAUD/WAADT, Hes-so VALAIS/WALLIS, HEI Sion, Infotronics. 2023
 * Created by Tristan Traiber
 * All rights reserved.
 */

#include <platform-config.h>

// ubxlib
#include <ubxlib.h>
#include "u_gnss_cfg.h"                     // u-bloc GNSS chip configuration file
#include "u_gnss_msg.h"                     // MCU <-> GNSS message exchange relative file
#include "u_cfg_app_platform_specific.h"    // Application platform specific configuration file
#include "platform-config.h"

#ifndef GNSS_CONFIG_H
#define GNSS_CONFIG_H

#define CONFIG_GNSS_LOG_LEVEL           3U
#define CONFIG_GNSS_MAX_OBSERVERS       2

#define GNSS_PWR_PIN                    DT_GPIO_PIN(DT_PATH(zephyr_user), gps_en_gpios)

#define GNSS_MODULE_TYPEM10             U_GNSS_MODULE_TYPE_M9   ///< u-blox M10 chip is equivalent to M9 chip
#define GNSS_I2C_ENABLE                 1                       ///< GNSS I2C enable, "-1" if not used
#define GNSS_I2C_SDA_PIN                I2C_SDA_PIN
#define GNSS_I2C_SCL_PIN                I2C_SCL_PIN
#define GNSS_I2C_ADDRESS                0x42                    ///< ublox MAX-M10S I2C Address

#define UBX_NAV_PVT_MESSAGE_LENGTH      (92 + U_UBX_PROTOCOL_OVERHEAD_LENGTH_BYTES)

#define RESET_GNSS_AFTER_CONNECT        0

#define GNSS_RESPONSE_WAIT_PERIODE      500

#define CFG_HW_RF_LNA_MODE              0x20a30057
#define GNSS_ENABLE_LNA_HIGH            0
#define GNSS_ENABLE_LNA_LOW             1
#define GNSS_EXTERNAL_ANT               2

//# define U_CFG_ENABLE_LOGGING         1

#endif