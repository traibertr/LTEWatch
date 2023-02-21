
/* **************************************************************************************************************** */
/* Header files                                                                                                     */
/* **************************************************************************************************************** */
#include "dispcontroller.h"

#include <spinlock.h>

/* **************************************************************************************************************** */
/* DEBUG COMPILATION OPTION & MACRO                                                                                 */
/* **************************************************************************************************************** */
#ifndef CONFIG_DIPS_LOG_LEVEL
    #define CONFIG_DIPS_LOG_LEVEL            4
#endif

#if (CONFIG_DIPS_LOG_LEVEL != 0)
    #include <logging/log.h>
    LOG_MODULE_REGISTER(display_ctrl, CONFIG_DIPS_LOG_LEVEL);

    #define DISP_DEBUG(args...)              LOG_DBG(args)
    #define DISP_ERROR(args...)              LOG_ERR(args)
    #define DISP_INFO(args...)               LOG_INF(args)
    #define DISP_WARN(args...)               LOG_WRN(args)
#else
    #define DISP_DEBUG(...)                  {(void) (0);}
    #define DISP_ERROR(...)                  {(void) (0);}
    #define DISP_INFO(...)                   {(void) (0);}
    #define DISP_WARN(...)                   {(void) (0);}
#endif


/* **************************************************************************************************************** */
/* STATIC ATTRIBUTE DECLARATION                                                                                     */
/* **************************************************************************************************************** */
static struct k_spinlock lock;

static const struct gpio_dt_spec dispEnSpec = GPIO_DT_SPEC_GET(DT_PATH(zephyr_user), disp_en_gpios);

static char batLvlStr[10]   = {0};
static char svsStr[10]      = {0};
static char timeStr[20]     = {0};
static char dateStr[20]     = {0};
static char mqttStr[10]     = {0};

static lv_obj_t *batt_label;
static lv_obj_t *svs_label;
static lv_obj_t *time_label;
static lv_obj_t *date_label;
static lv_obj_t *mqtt_label;
static lv_obj_t *mode_label;

/************************************************************************************************************/
/* Constructor | Destructor                                                                                 */
/************************************************************************************************************/


/************************************************************************************************************/
/*                                              PUBLIC SECTION                                              */
/************************************************************************************************************/
void DispController::init(void)
{
    gpio_pin_configure_dt(&dispEnSpec, GPIO_OUTPUT_HIGH);
    _startFlag = true;
}

// ------------------------------------------------------------------------------------------------------------------
void DispController::start(void)
{
    // Based on : zephyr/samples/subsys/display/lvgl/src/main.c
	const struct device *display_dev;

	display_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
	if (!device_is_ready(display_dev)) {
		LOG_ERR("Device not ready, aborting test");
		return;
	}
    LOG_INF("Device is starting");

	mode_label   = lv_label_create(lv_scr_act());

	lv_label_set_text(mode_label, "LTEWatch");
	lv_obj_align(mode_label, LV_ALIGN_CENTER, 0, 0);

    batt_label = lv_label_create(lv_scr_act());
    lv_obj_align(batt_label, LV_ALIGN_TOP_RIGHT, 0, 0);

    svs_label = lv_label_create(lv_scr_act());
    lv_obj_align(svs_label, LV_ALIGN_TOP_LEFT, 0, 0);

    time_label = lv_label_create(lv_scr_act());
    lv_obj_align(time_label, LV_ALIGN_BOTTOM_RIGHT, 0, 0);

    date_label = lv_label_create(lv_scr_act());
    lv_obj_align(date_label, LV_ALIGN_BOTTOM_LEFT, 0, 0);

    mqtt_label = lv_label_create(lv_scr_act());
    lv_obj_align(mqtt_label, LV_ALIGN_TOP_MID, -10, 0);

    sprintf(mqttStr, "...");
    lv_label_set_text(mqtt_label, mqttStr);

    sprintf(batLvlStr, LV_SYMBOL_BATTERY_EMPTY " ...%%");
    lv_label_set_text(batt_label, batLvlStr);

    sprintf(dateStr, "... / ... / ...");   // date dd/mm/yyyy
    lv_label_set_text(date_label, dateStr);

    sprintf(timeStr,  "... : ... : ...");    // time hh:mm:ss
    lv_label_set_text(time_label, timeStr);

    sprintf(svsStr, LV_SYMBOL_GPS "0"); // number of satelits (GNSS)
    lv_label_set_text(svs_label, svsStr);

}

// ------------------------------------------------------------------------------------------------------------------
void DispController::battDisp(uint8_t batteryLevelPercent, uint32_t batteryLevelMv, bool batMvMode, bool isBatteryCharging)
{
    static bool oldMode;
    static uint8_t oldBatLevel;
    int8_t diffBat = static_cast<int8_t>(batteryLevelPercent) - static_cast<int8_t>(oldBatLevel);
    if(diffBat < 0) diffBat*= -1;

     if (((oldBatLevel != batteryLevelPercent)&&(diffBat > 0))||(batMvMode != oldMode))
    {
        oldMode = batMvMode;
        oldBatLevel = batteryLevelPercent;
        if (batMvMode)
        {
            // Creat display text buffers
            if (isBatteryCharging)
            {
                sprintf(batLvlStr, LV_SYMBOL_CHARGE " %dmV", batteryLevelMv);
            }
            else if (batteryLevelPercent < 20)
            {
                sprintf(batLvlStr, LV_SYMBOL_BATTERY_EMPTY " %dmV", batteryLevelMv);
            }
            else if (batteryLevelPercent < 40)
            {
                sprintf(batLvlStr, LV_SYMBOL_BATTERY_1 " %dmV", batteryLevelMv);
            }
            else if (batteryLevelPercent < 50)
            {
                sprintf(batLvlStr, LV_SYMBOL_BATTERY_2 " %dmV", batteryLevelMv);
            }
            else if (batteryLevelPercent < 80)
            {
                sprintf(batLvlStr, LV_SYMBOL_BATTERY_3 " %dmV", batteryLevelMv);
            }
            else
            {
                sprintf(batLvlStr, LV_SYMBOL_BATTERY_FULL " %dmV", batteryLevelMv);
            }
            lv_obj_align(mqtt_label, LV_ALIGN_TOP_MID, -30, 0);
        }
        else
        {
            // Creat display text buffers
            if (isBatteryCharging)
            {
                sprintf(batLvlStr, LV_SYMBOL_CHARGE " %d%%", batteryLevelPercent);
            }
            else if (batteryLevelPercent < 20)
            {
                sprintf(batLvlStr, LV_SYMBOL_BATTERY_EMPTY " %d%%", batteryLevelPercent);
            }
            else if (batteryLevelPercent < 40)
            {
                sprintf(batLvlStr, LV_SYMBOL_BATTERY_1 " %d%%", batteryLevelPercent);
            }
            else if (batteryLevelPercent < 50)
            {
                sprintf(batLvlStr, LV_SYMBOL_BATTERY_2 " %d%%", batteryLevelPercent);
            }
            else if (batteryLevelPercent < 80)
            {
                sprintf(batLvlStr, LV_SYMBOL_BATTERY_3 " %d%%", batteryLevelPercent);
            }
            else
            {
                sprintf(batLvlStr, LV_SYMBOL_BATTERY_FULL " %d%%", batteryLevelPercent);
            }
             lv_obj_align(mqtt_label, LV_ALIGN_TOP_MID, -10, 0);
        }
        // Set display text
        lv_label_set_text(batt_label, batLvlStr);
    }
}

// ------------------------------------------------------------------------------------------------------------------
void DispController::gnssDisp(uint8_t svs)
{
    static uint8_t oldSvs;

    if (oldSvs != svs)
    {
        oldSvs = svs;
        // Creat display text buffers
        sprintf(svsStr, LV_SYMBOL_GPS " %d", svs); // number of satelits (GNSS)
        // Set display text
        lv_label_set_text(svs_label, svsStr);
    }
}
// ------------------------------------------------------------------------------------------------------------------
void DispController::timeDisp(uint8_t hour, uint8_t min, uint8_t sec)
{
    static uint8_t oldHour;
    static uint8_t oldMin;
    static uint8_t oldSec;

    if ((oldHour != hour)||(oldMin != min)||(oldSec != sec))
    {
        oldHour = hour;
        oldMin = min;
        oldSec = sec;

        // Creat display text buffers
        sprintf(timeStr,  "%02d:%02d:%02d", hour, min, sec);    // time hh:mm:ss
        // Set display text
        lv_label_set_text(time_label, timeStr);
    }
}

// ------------------------------------------------------------------------------------------------------------------
void DispController::dateDisp(uint16_t year, uint8_t month, uint8_t day)
{
    static uint8_t oldYear;
    static uint8_t oldMonth;
    static uint8_t oldDay;

    if ((oldYear != year)||(oldMonth != month)||(oldDay != day))
    {
        oldYear = year;
        oldMonth = month;
        oldDay = day;

        // Creat display text buffers
        sprintf(dateStr, "%02d/%02d/%02d", day, month, year);   // date dd/mm/yyyy
        // Set display text
        lv_label_set_text(date_label, dateStr);
    }
}

// ------------------------------------------------------------------------------------------------------------------
void DispController::mqttDisp(uint8_t state)
{
    // Creat display mqtt state text buffers
    switch (state)
    {
    case DISP_MQTT_CONNECTING_1:
         sprintf(mqttStr, LV_SYMBOL_REFRESH);
        break;
    case DISP_MQTT_CONNECTING_2:
         sprintf(mqttStr, LV_SYMBOL_REFRESH);
        break;
    case DISP_MQTT_CONNECTED:
        sprintf(mqttStr, LV_SYMBOL_WIFI);
        break;
    case DISP_MQTT_DISCONNECT:
        sprintf(mqttStr, LV_SYMBOL_CLOSE);
        break;
    case DISP_MQTT_SENDING:
        sprintf(mqttStr, LV_SYMBOL_UPLOAD);
        break;
    case DISP_MQTT_ERROR:
        sprintf(mqttStr, LV_SYMBOL_WARNING);
        break;
    default:
        break;
    }
    // Set display mqtt state text
    lv_label_set_text(mqtt_label, mqttStr);
}

// ------------------------------------------------------------------------------------------------------------------
void DispController::setMotDisp(uint8_t motorId)
{
	static lv_obj_t *set_motor_label;
    static lv_obj_t *motor_id_label;
    static char motorIdStr[20] = {0};

    if (_startFlag)
    {
        set_motor_label = lv_label_create(lv_scr_act());
        lv_label_set_text(set_motor_label, "SET MOTOR POS");
        lv_obj_align(set_motor_label, LV_ALIGN_TOP_MID, 0, 10);
        motor_id_label = lv_label_create(lv_scr_act());
        _startFlag = false;
    }

    switch (motorId)
    {
    case 0:
        sprintf(motorIdStr, LV_SYMBOL_SETTINGS " Second");
        break;
    case 1:
        sprintf(motorIdStr, LV_SYMBOL_SETTINGS " Minute");
        break;
    case 2:
        sprintf(motorIdStr, LV_SYMBOL_SETTINGS " Hour");
        break;
    default:
        break;
    }
    lv_label_set_text(motor_id_label, motorIdStr);
    lv_obj_align(motor_id_label, LV_ALIGN_CENTER, 0, 10);
}

// ------------------------------------------------------------------------------------------------------------------
void DispController::setGnssAnt(uint8_t gnssAntMode)
{
    static lv_obj_t *set_gnss_ant_label;
    static lv_obj_t *gnss_ant_id_label;
    static char gnssAntIdStr[20] = {0};

    if (_startFlag)
    {
        set_gnss_ant_label = lv_label_create(lv_scr_act());
        lv_label_set_text(set_gnss_ant_label, "SET GNSS ANT");
        lv_obj_align(set_gnss_ant_label, LV_ALIGN_TOP_MID, 0, 10);
        gnss_ant_id_label = lv_label_create(lv_scr_act());
        _startFlag = false;
    }

    switch (gnssAntMode)
    {
    case 0:
        sprintf(gnssAntIdStr, LV_SYMBOL_SETTINGS " On board");
        break;
    case 1:
        sprintf(gnssAntIdStr, LV_SYMBOL_SETTINGS " Not defined...");
        break;
    case 2:
        sprintf(gnssAntIdStr, LV_SYMBOL_SETTINGS " External");
        break;
    default:
        break;
    }
    lv_label_set_text(gnss_ant_id_label, gnssAntIdStr);
    lv_obj_align(gnss_ant_id_label, LV_ALIGN_CENTER, 0, 10);
}

// ------------------------------------------------------------------------------------------------------------------
void DispController::cleanDisp(void)
{
    lv_obj_clean(lv_scr_act());
    _startFlag = true;
}

// Singleton
DispController* DispController::getInstance(void)
{
    static DispController instance;
    return &instance;
}

void DispController::setModeDisp(char* modeStr)
{
    lv_label_set_text(mode_label, modeStr);
}

/************************************************************************************************************/
/*                                             PRIVATE SECTION                                              */
/************************************************************************************************************/