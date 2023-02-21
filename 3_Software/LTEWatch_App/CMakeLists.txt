# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

#list(APPEND ZEPHYR_EXTRA_MODULES "C:/ubxlib")

list(APPEND ZEPHYR_EXTRA_MODULES "${CMAKE_CURRENT_SOURCE_DIR}/src/library/mdw/partners/ubxlib")

message("========== INCLUDE ZEPHYR RTOS =========")
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(LTEWatch)

message("========== SET ENVIRONMENT VARIABLES =========")
set(ENV{MDW_MOTOR_PATH}     "src/library/mdw/partners/motor")
set(ENV{MDW_CALENDAR_PATH}  "src/library/mdw/partners/calendar")
set(ENV{MDW_BATTERY_PATH}   "src/library/mdw/partners/battery")
set(ENV{MDW_COMMON_PATH}    "src/library/mdw/common")
set(ENV{MDW_DISPLAY_PATH}   "src/library/mdw/common/display")
set(ENV{MDW_GNSS_PATH}      "src/library/mdw/common/gnss")
set(ENV{MDW_INTERFACE_PATH} "src/library/mdw/interface")
set(ENV{MCU_HAL_PATH}       "src/library/mcu/hal")
set(ENV{MCU_TOOLS_PATH}     "src/library/mcu/tools")
set(ENV{LIB_PATH}           "src/library")
set(ENV{CORE_PATH}          "src/core")
set(ENV{APP_PATH}           "src/app")
set(ENV{BUTTON_PATH}        "src/library/mdw/common/button")
set(ENV{GPIO_PATH}          "src/library/mdw/common/gpio")

message("========== INCLUDE PROJECT DIRECTORIES =========")

target_include_directories(app
  PRIVATE
    ##########################################
    #-------------- CORE & APP ---------------
    ##########################################
    $ENV{CORE_PATH}
    $ENV{APP_PATH}
    $ENV{APP_PATH}/config

    ##########################################
    #------------------ TOOLS ----------------
    ##########################################
    $ENV{MCU_TOOLS_PATH}

    ##########################################
    #------------------ HAL -----------------
    ##########################################
    $ENV{MCU_HAL_PATH}/critical
    $ENV{MCU_HAL_PATH}/gpio
    $ENV{MCU_HAL_PATH}/motor
    $ENV{MCU_HAL_PATH}/battery

    ##########################################
    #---------------- COMMON -----------------
    ##########################################
    $ENV{BUTTON_PATH}
    $ENV{BUTTON_PATH}/config
    $ENV{BUTTON_PATH}/interface
    $ENV{GPIO_PATH}
    $ENV{GPIO_PATH}/config
    $ENV{GPIO_PATH}/interface
    $ENV{MDW_COMMON_PATH}/core
    $ENV{MDW_COMMON_PATH}/pool
    $ENV{MDW_COMMON_PATH}/lte
    $ENV{MDW_COMMON_PATH}/mqtt
    $ENV{MDW_COMMON_PATH}/picoxf

    ##########################################
    #----------------- GNSS ------------------
    ##########################################
    $ENV{MDW_GNSS_PATH}

    ##########################################
    #----------------- DISPLAY ---------------
    ##########################################
    $ENV{MDW_DISPLAY_PATH}
    #$ENV{MDW_DISPLAY_PATH}/disp_driver

    $ENV{MDW_INTERFACE_PATH}

    ##########################################
    #----------------- BATTERY ---------------
    ##########################################
    $ENV{MDW_BATTERY_PATH}
    $ENV{MDW_BATTERY_PATH}/config
    $ENV{MDW_BATTERY_PATH}/driver

    ##########################################
    #----------------- MOTOR -----------------
    ##########################################
    $ENV{MDW_MOTOR_PATH}
    $ENV{MDW_MOTOR_PATH}/config
    $ENV{MDW_MOTOR_PATH}/controller
    $ENV{MDW_MOTOR_PATH}/interface
    $ENV{MDW_MOTOR_PATH}/view
    $ENV{MDW_MOTOR_PATH}/viewmodel
    $ENV{MDW_MOTOR_PATH}/XF

    ##########################################
    #----------------- CALENDAR --------------
    ##########################################
    $ENV{MDW_CALENDAR_PATH}/config
    $ENV{MDW_CALENDAR_PATH}/interface
    $ENV{MDW_CALENDAR_PATH}/XF
    $ENV{MDW_CALENDAR_PATH}

    ##########################################
    #---------------- PLATFORM ---------------
    ##########################################
    $ENV{LIB_PATH}/platform/config

    ##########################################
    #------------- project-config ------------
    ##########################################
    $ENV{LIB_PATH}/project-config

    ##########################################
    #------------------- XF ------------------
    ##########################################
    $ENV{LIB_PATH}/xf/adapters
    $ENV{LIB_PATH}/xf/config
    $ENV{LIB_PATH}/xf/core
    $ENV{LIB_PATH}/xf/queue
  )

message("========== ADD SOURCE FILES... =========")

target_sources(app
PRIVATE
###############################################################################################
#----------------------------------------- CORE -----------------------------------------------
#----------------------------------------------------------------------------------------------
$ENV{CORE_PATH}/main.cpp
$ENV{CORE_PATH}/factory.h                         $ENV{CORE_PATH}/factory.cpp

###############################################################################################
#----------------------------------------- APP ------------------------------------------------
#----------------------------------------------------------------------------------------------
$ENV{APP_PATH}/eventid.h
$ENV{APP_PATH}/appfactory.cpp
$ENV{APP_PATH}/picowatch.h                         $ENV{APP_PATH}/picowatch.cpp

###############################################################################################
#----------------------------------------- LIB CONFIG -----------------------------------------
###############################################################################################
#--- PLATFORM ---------------------------------------------------------------------------------
$ENV{LIB_PATH}/platform/config/memorypool-config.h
#--- APP --------------------------------------------------------------------------------------
$ENV{APP_PATH}/config/uicr-config.h
#--- PROJECT-CONFIG ---------------------------------------------------------------------------
$ENV{LIB_PATH}/project-config/platform-config.h
$ENV{LIB_PATH}/project-config/debug-config.h
#--- GPIO -------------------------------------------------------------------------------------
$ENV{GPIO_PATH}/config/gpio-config.h
#--- BUTTON -----------------------------------------------------------------------------------
$ENV{BUTTON_PATH}/config/button-config.h
#--- MOTOR ------------------------------------------------------------------------------------
$ENV{MDW_MOTOR_PATH}/config/motor-config.h

###############################################################################################
#----------------------------------------- LIB TOOLS ------------------------------------------
###############################################################################################
$ENV{MCU_TOOLS_PATH}/sdktools.h                   $ENV{MCU_TOOLS_PATH}/sdktools.cpp

###############################################################################################
#----------------------------------------- LIB HAL --------------------------------------------
###############################################################################################
$ENV{MCU_HAL_PATH}/motor/motordriverhal.h         $ENV{MCU_HAL_PATH}/motor/motordriverhal.cpp
$ENV{MCU_HAL_PATH}/gpio/gpiohal.h                 $ENV{MCU_HAL_PATH}/gpio/gpiohal.cpp

$ENV{MCU_HAL_PATH}/gpio/ledhal.h                  $ENV{MCU_HAL_PATH}/gpio/ledhal.cpp
$ENV{MCU_HAL_PATH}/critical/critical.h            $ENV{MCU_HAL_PATH}/critical/critical.c

$ENV{MCU_HAL_PATH}/battery/bathal.h               $ENV{MCU_HAL_PATH}/battery/bathal.cpp

###############################################################################################
#----------------------------------------- LIB BAT --------------------------------------------
###############################################################################################
$ENV{MDW_BATTERY_PATH}/batterymanager.h             $ENV{MDW_BATTERY_PATH}/batterymanager.cpp
$ENV{MDW_BATTERY_PATH}/ibatteryobserver.h
$ENV{MDW_BATTERY_PATH}/config/batt-config.h
$ENV{MDW_BATTERY_PATH}/driver/drv_bq25180.h

###############################################################################################
#----------------------------------------- LIB MOTOR ------------------------------------------
###############################################################################################
#--- MOTOR/CONTROLLER -------------------------------------------------------------------------
#$ENV{MDW_MOTOR_PATH}/controller/motorinitialisationcontroller.h
#$ENV{MDW_MOTOR_PATH}/controller/motorinitialisationcontroller.cpp
#--- MOTOR/INTERFACE --------------------------------------------------------------------------
$ENV{MDW_MOTOR_PATH}/interface/motorctrlobserver.h
$ENV{MDW_MOTOR_PATH}/interface/motordriverobserver.h
#$ENV{MDW_MOTOR_PATH}/interface/motorinitialisationviewmodelobserver.h
#--- MOTOR ------------------------------------------------------------------------------------
$ENV{MDW_MOTOR_PATH}/motor.h                      $ENV{MDW_MOTOR_PATH}/motor.cpp
$ENV{MDW_MOTOR_PATH}/motorcontroller.h            $ENV{MDW_MOTOR_PATH}/motorcontroller.cpp
$ENV{MDW_MOTOR_PATH}/motordriver.h                $ENV{MDW_MOTOR_PATH}/motordriver.cpp
$ENV{MDW_MOTOR_PATH}/motorid.h
$ENV{MDW_MOTOR_PATH}/motorpersistor.h
#--- MOTOR_XF ---------------------------------------------------------------------------------
$ENV{MDW_MOTOR_PATH}/XF/motorendevent.h
$ENV{MDW_MOTOR_PATH}/XF/motorevent.h
$ENV{MDW_MOTOR_PATH}/XF/MotorInitEvent.h

###############################################################################################
#----------------------------------------- LIB CALENDAR ----------------------------------------
###############################################################################################
$ENV{MDW_CALENDAR_PATH}/calendar.h                $ENV{MDW_CALENDAR_PATH}/calendar.cpp
$ENV{MDW_CALENDAR_PATH}/datetime.h                $ENV{MDW_CALENDAR_PATH}/datetime.cpp
#--- CONFIG -----------------------------------------------------------------------------------
$ENV{MDW_CALENDAR_PATH}/config/calendar-config.h
#--- INTERFACE --------------------------------------------------------------------------------
$ENV{MDW_CALENDAR_PATH}/interface/calendarobserver.h
#--- XF ---------------------------------------------------------------------------------------
$ENV{MDW_CALENDAR_PATH}/XF/calendarevent.h

###############################################################################################
#----------------------------------------- LIB CORE -------------------------------------------
#----------------------------------------------------------------------------------------------
$ENV{MDW_COMMON_PATH}/core/ibuffer.hpp
$ENV{MDW_COMMON_PATH}/core/icircbytebuffer.hpp
$ENV{MDW_COMMON_PATH}/core/islist.hpp
$ENV{MDW_COMMON_PATH}/core/isubject.hpp

###############################################################################################
#----------------------------------------- LIB DISPLAY ----------------------------------------
#----------------------------------------------------------------------------------------------
$ENV{MDW_DISPLAY_PATH}/config/disp-config.h
$ENV{MDW_DISPLAY_PATH}/dispcontroller.h                 $ENV{MDW_DISPLAY_PATH}/dispcontroller.cpp
#$ENV{MDW_DISPLAY_PATH}/disp_driver/SHARP_MIP.h   $ENV{MDW_DISPLAY_PATH}/disp_driver/SHARP_MIP.c
###############################################################################################
#----------------------------------------- LIB PICOXF -----------------------------------------
#----------------------------------------------------------------------------------------------
$ENV{MDW_COMMON_PATH}/picoxf/picoxf.h                $ENV{MDW_COMMON_PATH}/picoxf/picoxf.cpp
$ENV{MDW_COMMON_PATH}/picoxf/picoevent.h             $ENV{MDW_COMMON_PATH}/picoxf/picoevent.cpp
$ENV{MDW_COMMON_PATH}/picoxf/picoireactive.h
###############################################################################################
#----------------------------------------- LIB MQTT -------------------------------------------
#----------------------------------------------------------------------------------------------
$ENV{MDW_COMMON_PATH}/mqtt/imqttclientobserver.h
$ENV{MDW_COMMON_PATH}/mqtt/imqttobserver.h
$ENV{MDW_COMMON_PATH}/mqtt/mqttclient.h           $ENV{MDW_COMMON_PATH}/mqtt/mqttclient.cpp
$ENV{MDW_COMMON_PATH}/mqtt/mqttcontroller.h       $ENV{MDW_COMMON_PATH}/mqtt/mqttcontroller.cpp
$ENV{MDW_COMMON_PATH}/mqtt/config/mqtt-config.h
###############################################################################################
#----------------------------------------- LIB LTE  -------------------------------------------
#----------------------------------------------------------------------------------------------
$ENV{MDW_COMMON_PATH}/lte/ilteobserver.h
$ENV{MDW_COMMON_PATH}/lte/lte.h                               $ENV{MDW_COMMON_PATH}/lte/lte.cpp
$ENV{MDW_COMMON_PATH}/lte/config/lte-config.h
###############################################################################################
#----------------------------------------- LIB GNSS  ------------------------------------------
#----------------------------------------------------------------------------------------------
$ENV{MDW_GNSS_PATH}/ignssobserver.h
$ENV{MDW_GNSS_PATH}/gnsscontroller.h                $ENV{MDW_GNSS_PATH}/gnsscontroller.cpp
$ENV{MDW_GNSS_PATH}/ubxlib_custom/u_gnss_pos_date.h $ENV{MDW_GNSS_PATH}/ubxlib_custom/u_gnss_pos_date.c
###############################################################################################
#----------------------------------------- LIB INTERFACE --------------------------------------
#----------------------------------------------------------------------------------------------
#$ENV{MDW_INTERFACE_PATH}/functioncall.h
#$ENV{MDW_INTERFACE_PATH}/functionid.h
#$ENV{MDW_INTERFACE_PATH}/functionprovider.h
#$ENV{MDW_INTERFACE_PATH}/functionprovidercontext.h

###############################################################################################
#----------------------------------------- LIB XF ---------------------------------------------
#----------------------------------------------------------------------------------------------
$ENV{LIB_PATH}/xf/adapters/xf_adapter.h           $ENV{LIB_PATH}/xf/adapters/xf_adapter.cpp
#--- XF_CONFIG --------------------------------------------------------------------------------
$ENV{LIB_PATH}/xf/config/xf-config.h
#--- XF_CORE ----------------------------------------------------------------------------------
$ENV{LIB_PATH}/xf/core/initialevent.h             $ENV{LIB_PATH}/xf/core/initialevent.cpp
$ENV{LIB_PATH}/xf/core/ixfevent.h
$ENV{LIB_PATH}/xf/core/ixfreactive.h
$ENV{LIB_PATH}/xf/core/xf.h                       $ENV{LIB_PATH}/xf/core/xf.cpp
$ENV{LIB_PATH}/xf/core/xfevent.h                  $ENV{LIB_PATH}/xf/core/xfevent.cpp
$ENV{LIB_PATH}/xf/core/xfeventstatus.h
$ENV{LIB_PATH}/xf/core/xfnulltransition.h         $ENV{LIB_PATH}/xf/core/xfnulltransition.cpp
$ENV{LIB_PATH}/xf/core/xfreactive.h               $ENV{LIB_PATH}/xf/core/xfreactive.cpp
$ENV{LIB_PATH}/xf/core/xfthread.h                 $ENV{LIB_PATH}/xf/core/xfthread.cpp
$ENV{LIB_PATH}/xf/core/xftimeout.h                $ENV{LIB_PATH}/xf/core/xftimeout.cpp
$ENV{LIB_PATH}/xf/core/xftimeoutmanager.h         $ENV{LIB_PATH}/xf/core/xftimeoutmanager.cpp
#--- XF_QUEUE ----------------------------------------------------------------------------------
$ENV{LIB_PATH}/xf/queue/template-queue.h

###############################################################################################
#----------------------------------------- LIB POOL -------------------------------------------
#----------------------------------------------------------------------------------------------
$ENV{MDW_COMMON_PATH}/pool/config/memorypool-config-template.h
$ENV{MDW_COMMON_PATH}/pool/memorypool.h         $ENV{MDW_COMMON_PATH}/pool/memorypool.cpp
$ENV{MDW_COMMON_PATH}/pool/memorypoolmacros.h
$ENV{MDW_COMMON_PATH}/pool/objectpool.hpp
$ENV{MDW_COMMON_PATH}/pool/queue.hpp

###############################################################################################
#----------------------------------------- LIB CRITICAL ---------------------------------------
#----------------------------------------------------------------------------------------------
$ENV{MCU_HAL_PATH}/critical/critical.h          $ENV{MCU_HAL_PATH}/critical/critical.c

###############################################################################################
#----------------------------------------- LIB BUTTON -----------------------------------------
#----------------------------------------------------------------------------------------------
$ENV{BUTTON_PATH}/interface/buttonobserver.h
$ENV{BUTTON_PATH}/button.h                      $ENV{BUTTON_PATH}/button.cpp
$ENV{BUTTON_PATH}/buttoncontroller.h            $ENV{BUTTON_PATH}/buttoncontroller.cpp
$ENV{BUTTON_PATH}/buttonid.h
$ENV{BUTTON_PATH}/buttonpersistor.h

###############################################################################################
#----------------------------------------- LIB GPIO -------------------------------------------
#----------------------------------------------------------------------------------------------
$ENV{GPIO_PATH}/interface/gpioobserver.h
$ENV{GPIO_PATH}/XF/gpioevent.h
$ENV{GPIO_PATH}/XF/gpiofallingevent.h
$ENV{GPIO_PATH}/XF/gpiorisingevent.h
$ENV{GPIO_PATH}/basecontroller.h              $ENV{GPIO_PATH}/basecontroller.cpp
$ENV{GPIO_PATH}/gpio.h                        $ENV{GPIO_PATH}/gpio.cpp
$ENV{GPIO_PATH}/gpiocontroller.h              $ENV{GPIO_PATH}/gpiocontroller.cpp
$ENV{GPIO_PATH}/gpioid.h
$ENV{GPIO_PATH}/ledcontroller.h               $ENV{GPIO_PATH}/ledcontroller.cpp

)
message("========== CMakeLists.txt DONE ! =========")