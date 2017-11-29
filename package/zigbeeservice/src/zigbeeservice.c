/*************************************************************************
    > File Name: zigbeeservice.c
    > Author: lunan
    > Mail: 6616@shuncom.com 
    > Created Time: 2016年02月27日 星期六 13时16分30秒
 ************************************************************************/

#include <stdio.h>
#include "zigbeezap.h"
#include "user_api.h"
#include "user_cb.h"
#include "ubus.h"
#include "zha_strategy.h"
#include "user_types.h"
#include "types.h"
#include "mqtt.h"
#include <openssl/md5.h>

#include "cJSON.h"
#include "defines.h"

#define SUCCESS 1
#define FAILURE 0

#define WRITE_CONTROL 0
#define CMD_CONTROL 0

#define CMD_SUCCESS 0
#define CMD_FAILURE 1
#define CMD_PARA_ERROR 2
#define CMD_MALLOC_ERROR 3
#define CMD_NOT_FIND_METHOD 4

#define IEEE_ADDR_LEN 8

#define TYPE_BOOL 0
#define TYPE_STRING 1
#define TYPE_BYTE 2
#define TYPE_SHORT 3
#define TYPE_THREE 4
#define TYPE_INT 5
#define TYPE_LL 6
#define TYPE_SHORT_STRING 7
#define TYPE_THREE_STRING 8
#define TYPE_INT_STRING 9
#define TYPE_LL_STRING 10

#define END_FLAG_BUF "xx"

#define VISIBLE 0
#define DISVISIBLE 1

/*************************basic****************************/
#define ATTRID_BASIC_ZCL_VERSION 0x0000
#define ATTRID_BASIC_APPL_VERSION 0x0001
#define ATTRID_BASIC_STACK_VERSION 0x0002
#define ATTRID_BASIC_HW_VERSION 0x0003
#define ATTRID_BASIC_MANUFACTURER_NAME 0x0004
#define ATTRID_BASIC_MODEL_ID 0x0005
#define ATTRID_BASIC_DATE_CODE 0x0006
#define ATTRID_BASIC_POWER_SOURCE 0x0007
#define ATTRID_BASIC_APPLICATION_PROFILE_VERSION 0x0008
#define ATTRID_BASIC_SW_BUILD_ID 0x4000

/***************************on off*****************************/
#define ATTRID_ON_OFF 0x0000
#define ATTRID_ON_OFF_GLOBAL_SCENE_CTRL 0x4000
#define ATTRID_ON_OFF_ON_TIME 0x4001
#define ATTRID_ON_OFF_OFF_WAIT_TIME 0x4002

/*************************on off switch************************/
#define ATTRID_ON_OFF_SWITCH_TYPE 0x0000
#define ATTRID_ON_OFF_SWITCH_ACTIONS 0x0010

/*********************************************************/
#define ATTRID_SE_METERING_CURR_SUMM_DLVD 0x0000
#define ATTRID_SE_METERING_STATUS 0x0200
#define ATTRID_SE_METERING_UOM 0x0300
#define ATTRID_SE_METERING_MULT 0x0301
#define ATTRID_SE_METERING_DIV 0x0302
#define ATTRID_SE_METERING_SUMM_FMTG 0x0303
#define ATTRID_SE_METERING_DEVICE_TYPE 0x0306

/***********************pm25******************************/
#define ATTRID_IOV_BASIC_STATE_TEXT 0x000E

/*************************zone****************************/
#define ATTRID_SS_ZONE_ID 0x0011
#define ATTRID_SS_IAS_ZONE_TYPE 0x0001
#define ATTRID_SS_IAS_ZONE_STATUS 0x0002

/***********************battery******************************/
#define ATTRID_POWER_CFG_BAT_SIZE 0x0031
#define ATTRID_POWER_CFG_MAINS_ALARM_MASK 0x0010

/**************************door lock****************************/
#define ATTRID_CLOSURES_LOCK_STATE 0x0000
#define ATTRID_CLOSURES_LOCK_TYPE 0x0001
#define ATTRID_CLOSURES_ACTUATOR_ENABLED 0x0002

/**************************************************************************/
/***          HVAC: Thermostat Cluster Attributes                       ***/
/**************************************************************************/
// Thermostat information attribute set
#define ATTRID_HVAC_THERMOSTAT_LOCAL_TEMPERATURE 0x0000
#define ATTRID_HVAC_THERMOSTAT_RUNNING_STATE 0x0029
#define ATTRID_HVAC_THERMOSTAT_OCCUPIED_COOLING_SETPOINT 0x0011
#define ATTRID_HVAC_THERMOSTAT_OCCUPIED_HEATING_SETPOINT 0x0012
#define ATTRID_HVAC_THERMOSTAT_ALAM_MASK 0x001D

/****************************************/
/*** Level Control Cluster Attributes ***/
/****************************************/
#define ATTRID_LEVEL_CURRENT_LEVEL 0x0000
#define ATTRID_LEVEL_REMAINING_TIME 0x0001
#define ATTRID_LEVEL_ON_OFF_TRANSITION_TIME 0x0010
#define ATTRID_LEVEL_ON_LEVEL 0x0011
#define ATTRID_LEVEL_ON_TRANSITION_TIME 0x0012
#define ATTRID_LEVEL_OFF_TRANSITION_TIME 0x0013
#define ATTRID_LEVEL_DEFAULT_MOVE_RATE 0x0014

/*********************fan***********************************/
#define ATTRID_HVAC_FAN_CTRL_FAN_MODE 0x0000 // M, R/W, ENUM8
#define ATTRID_HVAC_FAN_CTRL_FAN_SEQUENCE 0x0001

/*****************************wimdow*******************************/
#define ATTRSET_WINDOW_COVERING_INFO 0x0000
#define ATTRSET_WINDOW_COVERING_SETTINGS 0x0010

#define ATTRID_CLOSURES_CURRENT_POSITION_LIFT_PERCENTAGE (ATTRSET_WINDOW_COVERING_INFO + 0x0008)
#define ATTRID_CLOSURES_WINDOW_COVERING_TYPE (ATTRSET_WINDOW_COVERING_INFO + 0x0000)
#define ATTRID_CLOSURES_WINDOW_COVERING_MODE (ATTRSET_WINDOW_COVERING_SETTINGS + 0x0007)

/*******************A_ELECTRICAL_MEASUREMENT******************/
#define ATTRID_ELECTRICAL_MEASUREMENT_RMS_VOLTAGE 0x0505
#define ATTRID_ELECTRICAL_MEASUREMENT_RMS_CURRENT 0x0508
#define ATTRID_ELECTRICAL_MEASUREMENT_ACTIVE_POWER 0x050B
#define ATTRID_ELECTRICAL_MEASUREMENT_POWER_FACTOR 0x0510
#define ATTRID_ELECTRICAL_MEASUREMENT_REACTIVE_POWER 0x050E

/*******************************/
/*** On/Off Cluster Commands ***/
/*******************************/
#define COMMAND_OFF 0x00
#define COMMAND_ON 0x01
#define COMMAND_TOGGLE 0x02

// Server commands received (Client-to-Server in ZCL Header)
#define COMMAND_SS_IAS_WD_START_WARNING 0x00
#define COMMAND_SS_IAS_WD_SQUAWK 0x01

/***********************************/
/*** Identify Cluster Attributes ***/
/***********************************/
#define ATTRID_IDENTIFY_TIME 0x0000
#define ATTRID_IDENTIFY_COMMISSION_STATE 0x0001

/*********************************************/
/***  Appliance Control Cluster Attributes ***/
/********************************************/

// Server Attributes
#define ATTRID_APPLIANCE_CONTROL_START_TIME 0x0000	 // M, R, UINT16
#define ATTRID_APPLIANCE_CONTROL_FINISH_TIME 0x0001	// M, R, UINT16
#define ATTRID_APPLIANCE_CONTROL_REMAINING_TIME 0x0002 // O, R, UINT16

/*****************************************************************************/
/***         Occupancy Sensing Cluster Attributes                          ***/
/*****************************************************************************/
// Occupancy Sensor Configuration attribute set
#define ATTRID_MS_OCCUPANCY_SENSING_CONFIG_OCCUPANCY 0x0000				// M, R, BITMAP8
#define ATTRID_MS_OCCUPANCY_SENSING_CONFIG_OCCUPANCY_SENSOR_TYPE 0x0001 // M, R, ENUM8

// PIR Configuration attribute set
#define ATTRID_MS_OCCUPANCY_SENSING_CONFIG_PIR_O_TO_U_DELAY 0x0010  // O, R/W, UINT16
#define ATTRID_MS_OCCUPANCY_SENSING_CONFIG_PIR_U_TO_O_DELAY 0x0011  // O, R/W, UINT16
#define ATTRID_MS_OCCUPANCY_SENSING_CONFIG_PIR_U_TO_O_THRESH 0x0012 // O, R/W, UINT8

// Ultrasonic Configuration attribute set
#define ATTRID_MS_OCCUPANCY_SENSING_CONFIG_ULTRASONIC_O_TO_U_DELAY 0x0020  // O, R/W, UINT16
#define ATTRID_MS_OCCUPANCY_SENSING_CONFIG_ULTRASONIC_U_TO_O_DELAY 0x0021  // O, R/W, UINT16
#define ATTRID_MS_OCCUPANCY_SENSING_CONFIG_ULTRASONIC_U_TO_O_THRESH 0x0022 // O, R/W, UINT8

/********************************/
/*** Group Cluster Attributes ***/
/********************************/
#define ATTRID_GROUP_NAME_SUPPORT 0x0000

/*********************************/
/*** Scenes Cluster Attributes ***/
/*********************************/
// Scene Management Information
#define ATTRID_SCENES_COUNT 0x0000
#define ATTRID_SCENES_CURRENT_SCENE 0x0001
#define ATTRID_SCENES_CURRENT_GROUP 0x0002
#define ATTRID_SCENES_SCENE_VALID 0x0003
#define ATTRID_SCENES_NAME_SUPPORT 0x0004
#define ATTRID_SCENES_LAST_CFG_BY 0x0005

typedef struct
{
	unsigned short int attribute_id;
	unsigned short int data_type;
	unsigned short int visible;
	unsigned char key[20];
} attribute_info;

typedef struct
{
	unsigned short int cluaster_id;
	attribute_info *attributes;
} cluaster_info;

attribute_info BASIC[] = {
	{ATTRID_BASIC_HW_VERSION, TYPE_BYTE, VISIBLE, "hver"},
	{ATTRID_BASIC_ZCL_VERSION, TYPE_BYTE, VISIBLE, "sver"},
	{ATTRID_BASIC_MANUFACTURER_NAME, TYPE_STRING, VISIBLE, "fac"},
	{ATTRID_BASIC_MODEL_ID, TYPE_STRING, VISIBLE, "dsp"},
	{ATTRID_BASIC_DATE_CODE, TYPE_STRING, VISIBLE, "date"},
	{ATTRID_BASIC_POWER_SOURCE, TYPE_BYTE, VISIBLE, "ps"},
	{ATTRID_BASIC_SW_BUILD_ID, TYPE_STRING, VISIBLE, "swid"},
	{0, 0, VISIBLE, END_FLAG_BUF}};

attribute_info ON_OFF[] = {
	{ATTRID_ON_OFF, TYPE_BOOL, VISIBLE, "on"},
	{0x8000, TYPE_BOOL, VISIBLE, "childlock"},
	{0x8001, TYPE_BYTE, VISIBLE, "backlight"},
	{0xf000, TYPE_BYTE, VISIBLE, "plug"},
	{0, 0, VISIBLE, END_FLAG_BUF}};

attribute_info ON_OFF_SWITCH_CONFIG[] = {
	{ATTRID_ON_OFF_SWITCH_TYPE, TYPE_BOOL, VISIBLE, "on"},
	{ATTRID_ON_OFF_SWITCH_ACTIONS, TYPE_BYTE, VISIBLE, "act"},
	{0, 0, VISIBLE, END_FLAG_BUF}};

attribute_info SE_SIMPLE_METERING[] = {
	{ATTRID_SE_METERING_CURR_SUMM_DLVD, TYPE_LL, VISIBLE, "energy"},
	{ATTRID_SE_METERING_STATUS, TYPE_LL, VISIBLE, "sta"},
	{ATTRID_SE_METERING_UOM, TYPE_BYTE, VISIBLE, "unit"},
	{ATTRID_SE_METERING_MULT, TYPE_THREE, VISIBLE, "mult"},
	{ATTRID_SE_METERING_DIV, TYPE_THREE, VISIBLE, "div"},
	{ATTRID_SE_METERING_SUMM_FMTG, TYPE_BYTE, VISIBLE, "sum"},
	{ATTRID_SE_METERING_DEVICE_TYPE, TYPE_BYTE, VISIBLE, "type"},
	{0, 0, VISIBLE, END_FLAG_BUF}};

attribute_info HA_ELECTRICAL_MEASUREMENT[] = {
	{ATTRID_ELECTRICAL_MEASUREMENT_RMS_VOLTAGE, TYPE_SHORT, VISIBLE, "volt"},
	{ATTRID_ELECTRICAL_MEASUREMENT_RMS_CURRENT, TYPE_SHORT, VISIBLE, "curr"},
	{ATTRID_ELECTRICAL_MEASUREMENT_ACTIVE_POWER, TYPE_SHORT, VISIBLE, "actp"},
	{ATTRID_ELECTRICAL_MEASUREMENT_REACTIVE_POWER, TYPE_SHORT, VISIBLE, "reactp"},
	{ATTRID_ELECTRICAL_MEASUREMENT_POWER_FACTOR, TYPE_BYTE, VISIBLE, "facp"},
	{0, 0, VISIBLE, END_FLAG_BUF}};

attribute_info HVAC_FAN_CONTROL[] = {
	{ATTRID_HVAC_FAN_CTRL_FAN_MODE, TYPE_BYTE, VISIBLE, "fanmode"},
	{ATTRID_HVAC_FAN_CTRL_FAN_SEQUENCE, TYPE_BYTE, VISIBLE, "fanseq"},
	{0x8000, TYPE_BYTE, VISIBLE, "powermode"},
	{0, 0, VISIBLE, END_FLAG_BUF}};

attribute_info MS_TEMPERATURE_MEASUREMENT[] = {
	{ATTRID_MS_TEMPERATURE_MEASURED_VALUE, TYPE_SHORT_STRING, VISIBLE, "meatemp"},
	{ATTRID_MS_TEMPERATURE_MIN_MEASURED_VALUE, TYPE_SHORT_STRING, VISIBLE, "minmeatemp"},
	{ATTRID_MS_TEMPERATURE_MAX_MEASURED_VALUE, TYPE_SHORT_STRING, VISIBLE, "maxmeatemp"},
	{0, 0, VISIBLE, END_FLAG_BUF}};

attribute_info MS_RELATIVE_HUMIDITY[] = {
	{ATTRID_MS_RELATIVE_HUMIDITY_MEASURED_VALUE, TYPE_SHORT_STRING, VISIBLE, "humi"},
	{ATTRID_MS_RELATIVE_HUMIDITY_MIN_MEASURED_VALUE, TYPE_SHORT_STRING, VISIBLE, "minhumi"},
	{ATTRID_MS_RELATIVE_HUMIDITY_MAX_MEASURED_VALUE, TYPE_SHORT_STRING, VISIBLE, "maxhumi"},
	{ATTRID_MS_RELATIVE_HUMIDITY_TOLERANCE, TYPE_SHORT, VISIBLE, "allowerror"},
	{0, 0, VISIBLE, END_FLAG_BUF}};

attribute_info GEN_ANALOG_INPUT_BASIC[] = {
	{ATTRID_IOV_BASIC_STATE_TEXT, TYPE_SHORT, VISIBLE, "pm25"},
	{ATTRID_IOV_BASIC_STATE_TEXT, TYPE_SHORT, VISIBLE, "voc"}, //////////ep
	{0, 0, VISIBLE, END_FLAG_BUF}};

attribute_info SS_IAS_ZONE[] = {
	{ATTRID_SS_IAS_ZONE_STATUS, TYPE_SHORT, VISIBLE, "sta"},
	{ATTRID_SS_IAS_ZONE_TYPE, TYPE_SHORT, VISIBLE, "type"},
	{ATTRID_SS_ZONE_ID, TYPE_BYTE, VISIBLE, "zid"},
	{0, 0, VISIBLE, END_FLAG_BUF}};

attribute_info SS_IAS_WD[] = {
	{0, 0, VISIBLE, END_FLAG_BUF}};

attribute_info MS_ILLUMINANCE_MEASUREMENT[] = {
	{ATTRID_MS_ILLUMINANCE_MEASURED_VALUE, TYPE_SHORT_STRING, VISIBLE, "nlux"},
	{ATTRID_MS_ILLUMINANCE_MIN_MEASURED_VALUE, TYPE_SHORT, VISIBLE, "minlux"},
	{ATTRID_MS_ILLUMINANCE_MAX_MEASURED_VALUE, TYPE_SHORT, VISIBLE, "maxlux"},
	{0, 0, VISIBLE, END_FLAG_BUF}};

attribute_info MS_ILLUMINANCE_LEVEL_SENSING_CONFIG[] = {
	{ATTRID_MS_ILLUMINANCE_LEVEL_STATUS, TYPE_BYTE, VISIBLE, "llux"},
	{ATTRID_MS_ILLUMINANCE_TARGET_LEVEL, TYPE_SHORT_STRING, VISIBLE, "tlux"},
	{0, 0, VISIBLE, END_FLAG_BUF}};

attribute_info GEN_POWER_CFG[] = {
	{ATTRID_POWER_CFG_BAT_SIZE, TYPE_BYTE, VISIBLE, "bats"},
	{ATTRID_POWER_CFG_MAINS_ALARM_MASK, TYPE_BYTE, VISIBLE, "alm"},
	{0x0021, TYPE_BYTE, VISIBLE, "batpt"},
	{0, 0, VISIBLE, END_FLAG_BUF}};

attribute_info CLOSURES_DOOR_LOCK[] = {
	{ATTRID_CLOSURES_LOCK_STATE, TYPE_BYTE, VISIBLE, "sta"},
	{ATTRID_CLOSURES_LOCK_TYPE, TYPE_BYTE, VISIBLE, "type"},
	{ATTRID_CLOSURES_ACTUATOR_ENABLED, TYPE_BOOL, VISIBLE, "en"},
	{0x0040, TYPE_SHORT, VISIBLE, "alm"},
	{0, 0, VISIBLE, END_FLAG_BUF}};

attribute_info HVAC_THERMOSTAT[] = {
	{ATTRID_HVAC_THERMOSTAT_ALAM_MASK, TYPE_BYTE, VISIBLE, "tempalm"},
	{ATTRID_HVAC_THERMOSTAT_LOCAL_TEMPERATURE, TYPE_SHORT_STRING, VISIBLE, "tgtemp"},
	{ATTRID_HVAC_THERMOSTAT_OCCUPIED_COOLING_SETPOINT, TYPE_SHORT_STRING, VISIBLE, "coolset"},
	{ATTRID_HVAC_THERMOSTAT_OCCUPIED_HEATING_SETPOINT, TYPE_SHORT_STRING, VISIBLE, "heatset"},
	{0x8000, TYPE_BYTE, VISIBLE, "NumOfAirconditions"},
	{0x001c, TYPE_BYTE, VISIBLE, "workmode"},
	{0, 0, VISIBLE, END_FLAG_BUF}};

attribute_info GEN_LEVEL_CONTROL[] = {
	{ATTRID_LEVEL_CURRENT_LEVEL, TYPE_BYTE, VISIBLE, "bri"},
	{ATTRID_LEVEL_REMAINING_TIME, TYPE_SHORT, VISIBLE, "remaintime"},
	{ATTRID_LEVEL_ON_OFF_TRANSITION_TIME, TYPE_SHORT, VISIBLE, "onofftran"},
	{ATTRID_LEVEL_ON_LEVEL, TYPE_BYTE, VISIBLE, "lonl"},
	{ATTRID_LEVEL_ON_TRANSITION_TIME, TYPE_SHORT, VISIBLE, "opntran"},
	{ATTRID_LEVEL_OFF_TRANSITION_TIME, TYPE_SHORT, VISIBLE, "clstran"},
	{ATTRID_LEVEL_DEFAULT_MOVE_RATE, TYPE_BYTE, VISIBLE, "rate"},
	{0, 0, VISIBLE, END_FLAG_BUF}};

attribute_info CLOSURES_WINDOW_COVERING[] = {
	{ATTRID_CLOSURES_CURRENT_POSITION_LIFT_PERCENTAGE, TYPE_BYTE, VISIBLE, "pt"},
	{ATTRID_CLOSURES_WINDOW_COVERING_TYPE, TYPE_BYTE, VISIBLE, "type"},
	{ATTRID_CLOSURES_WINDOW_COVERING_MODE, TYPE_BYTE, VISIBLE, "workmode"},
	{0, 0, VISIBLE, END_FLAG_BUF}};

attribute_info LIGHTING_COLOR_CONTROL[] = {
	{0x0000, TYPE_BYTE, VISIBLE, "hue"},
	{0x0001, TYPE_BYTE, VISIBLE, "sat"},
	{0x0007, TYPE_SHORT, VISIBLE, "ctp"},
	{0, 0, VISIBLE, END_FLAG_BUF}};

attribute_info AL_FE01[] = {
	{0X2001, TYPE_BYTE, VISIBLE, "lqi"},
	{0, 0, VISIBLE, END_FLAG_BUF}};

attribute_info AL_FE02[] = {
	{0X0000, TYPE_SHORT, VISIBLE, "pm25"},
	{0, 0, VISIBLE, END_FLAG_BUF}};

attribute_info AL_FE03[] = {
	{0X0000, TYPE_SHORT, VISIBLE, "CO2"},
	{0, 0, VISIBLE, END_FLAG_BUF}};

attribute_info AL_FE04[] = {
	{0X0000, TYPE_SHORT, VISIBLE, "formaldehyde"},
	{0, 0, VISIBLE, END_FLAG_BUF}};

attribute_info AL_FE05[] = {
	{0X0000, TYPE_BYTE, VISIBLE, "val"},
	{0, 0, VISIBLE, END_FLAG_BUF}};

attribute_info APPLIANCE_CONTROL[] = {
	{ATTRID_APPLIANCE_CONTROL_START_TIME, TYPE_SHORT, VISIBLE, "starttime"},
	{0, 0, VISIBLE, END_FLAG_BUF}};

attribute_info OCCUPANCY_SENSING[] = {
	{ATTRID_MS_OCCUPANCY_SENSING_CONFIG_OCCUPANCY, TYPE_BYTE, VISIBLE, "occupancy"},
	{0, 0, VISIBLE, END_FLAG_BUF}};

attribute_info BINARY_INPUT_BASIC[] = {
	{0x0000, TYPE_BYTE, VISIBLE, "plugval2"},
	{0, 0, VISIBLE, END_FLAG_BUF}};

attribute_info ANALOG_INPUT_BASIC[] = {
	{0x0000, TYPE_BYTE, VISIBLE, "plugval1"},
	{0x000e, TYPE_SHORT, VISIBLE, "ANAINPUT0e"},
	{0, 0, VISIBLE, END_FLAG_BUF}};

attribute_info IDENTIFY[] = {
	{ATTRID_IDENTIFY_TIME, TYPE_SHORT, VISIBLE, "identifytime"},
	{0, 0, VISIBLE, END_FLAG_BUF}};

attribute_info GROUPS[] = {
	{ATTRID_GROUP_NAME_SUPPORT, TYPE_BYTE, VISIBLE, "namesupport"},
	{0, 0, VISIBLE, END_FLAG_BUF}};

attribute_info SCENES[] = {
	{ATTRID_SCENES_COUNT, TYPE_BYTE, VISIBLE, "scenecount"},
	{0, 0, VISIBLE, END_FLAG_BUF}};

attribute_info GEN_TIME[] = {
	{0, 0, VISIBLE, END_FLAG_BUF}};

attribute_info OTA[] = {
	{0, 0, VISIBLE, END_FLAG_BUF}};

cluaster_info cluasters[] = {
	{ZCL_CLUSTER_ID_GEN_BASIC, BASIC},
	{ZCL_CLUSTER_ID_GEN_POWER_CFG, GEN_POWER_CFG},
	{ZCL_CLUSTER_ID_GEN_ON_OFF, ON_OFF},
	{ZCL_CLUSTER_ID_GEN_ON_OFF_SWITCH_CONFIG, ON_OFF_SWITCH_CONFIG},
	{ZCL_CLUSTER_ID_SE_SIMPLE_METERING, SE_SIMPLE_METERING},
	{ZCL_CLUSTER_ID_GEN_LEVEL_CONTROL, GEN_LEVEL_CONTROL},
	{ZCL_CLUSTER_ID_GEN_ANALOG_INPUT_BASIC, GEN_ANALOG_INPUT_BASIC},
	{ZCL_CLUSTER_ID_CLOSURES_DOOR_LOCK, CLOSURES_DOOR_LOCK},
	{ZCL_CLUSTER_ID_CLOSURES_WINDOW_COVERING, CLOSURES_WINDOW_COVERING},
	{ZCL_CLUSTER_ID_HVAC_THERMOSTAT, HVAC_THERMOSTAT},
	{ZCL_CLUSTER_ID_HVAC_FAN_CONTROL, HVAC_FAN_CONTROL},
	{ZCL_CLUSTER_ID_LIGHTING_COLOR_CONTROL, LIGHTING_COLOR_CONTROL},
	{ZCL_CLUSTER_ID_MS_ILLUMINANCE_MEASUREMENT, MS_ILLUMINANCE_MEASUREMENT},
	{ZCL_CLUSTER_ID_MS_ILLUMINANCE_LEVEL_SENSING_CONFIG, MS_ILLUMINANCE_LEVEL_SENSING_CONFIG},
	{ZCL_CLUSTER_ID_MS_TEMPERATURE_MEASUREMENT, MS_TEMPERATURE_MEASUREMENT},
	{ZCL_CLUSTER_ID_MS_RELATIVE_HUMIDITY, MS_RELATIVE_HUMIDITY},
	{ZCL_CLUSTER_ID_SS_IAS_ZONE, SS_IAS_ZONE},
	{ZCL_CLUSTER_ID_SS_IAS_WD, SS_IAS_WD},
	{ZCL_CLUSTER_ID_HA_ELECTRICAL_MEASUREMENT, HA_ELECTRICAL_MEASUREMENT},
	{0xfe01, AL_FE01},
	{0xfe02, AL_FE02},
	{0xfe03, AL_FE03},
	{0xfe04, AL_FE04},
	{0xfe05, AL_FE05},

	{ZCL_CLUSTER_ID_GEN_APPLIANCE_CONTROL, APPLIANCE_CONTROL},
	{ZCL_CLUSTER_ID_MS_OCCUPANCY_SENSING, OCCUPANCY_SENSING},
	{ZCL_CLUSTER_ID_GEN_BINARY_INPUT_BASIC, BINARY_INPUT_BASIC},
	{ZCL_CLUSTER_ID_GEN_ANALOG_INPUT_BASIC, ANALOG_INPUT_BASIC},

	{ZCL_CLUSTER_ID_GEN_IDENTIFY, IDENTIFY},
	{ZCL_CLUSTER_ID_GEN_GROUPS, GROUPS},
	{ZCL_CLUSTER_ID_GEN_SCENES, SCENES},

	{ZCL_CLUSTER_ID_GEN_TIME, GEN_TIME},
	{ZCL_CLUSTER_ID_OTA, OTA},
	{0, (attribute_info *)NULL},
};

unsigned short HA_PROFILE_DEVICE[] = {
	ZCL_HA_DEVICEID_DIMMABLE_LIGHT,
	ZCL_HA_DEVICEID_ON_OFF_LIGHT,
	ZCL_HA_DEVICEID_COLORED_DIMMABLE_LIGHT,
	ZCL_HA_DEVICEID_MAINS_POWER_OUTLET,
	ZCL_HA_DEVICEID_REMOTE_CONTROL,
	ZCL_HA_DEVICEID_DOOR_LOCK,
	ZCL_HA_DEVICEID_ON_OFF_SWITCH,
	ZCL_HA_DEVICEID_SMART_PLUG,
	ZCL_HA_DEVICEID_WHITE_GOODS,
	ZCL_HA_DEVICEID_SIMPLE_SENSOR,
	ZCL_HA_DEVICEID_LIGHT_SENSOR,
	ZCL_HA_DEVICEID_TEMPERATURE_SENSOR,
	ZCL_HA_DEVICEID_PUMP,
	ZCL_HA_DEVICEID_HEATING_COOLING_UNIT,
	ZCL_HA_DEVICEID_THERMOSTAT,
	ZCL_HA_DEVICEID_IAS_ZONE,
	ZCL_HA_DEVICEID_WINDOW_COVERING_DEVICE,
	ZCL_HA_DEVICEID_SCENE_SELECTOR,
	ZCL_HA_DEVICEID_WINDOW_COVERING_CONTROLLER,
	ZCL_HA_DEVICEID_DIMMER_SWITCH,
	0xffff};

unsigned short ZLL_PROFILE_DEVICE[] = {
	ZLL_DEVICEID_COLOR_LIGHT,
	ZLL_DEVICEID_EXTENDED_COLOR_LIGHT,
	ZLL_DEVICEID_DIMMABLE_LIGHT,
	ZLL_DEVICEID_COLOR_TEMPERATURE_LIGHT,
	0xffff};

unsigned short SHUNCOM_PROFILE_ID[] = {
	ZCL_SHUNCOM_DEVICEID_TARNSPORT,
	ZCL_SHUNCOM_DEVICEID_SMART_TARNSPORT,
	0xffff};

int local_is_lvmi_device(cJSON *p_device)
{
	char *temp = cJSON_Print(p_device);
	if (temp)
		debug(DEBUG_ERROR, "p_device[%s]", temp);
	else
		debug(DEBUG_ERROR, "temp is NULL");
	free(temp);
	temp = NULL;

	cJSON *p_dsp = cJSON_GetObjectItem(p_device, "dsp");

	if (!p_dsp)
	{
		debug(DEBUG_ERROR, "p_dsp is NULL");
		return FAILURE;
	}
	if ((strlen(p_dsp->valuestring) >= 4) && (memcmp(p_dsp->valuestring, "lumi", 4) == 0))
		return SUCCESS;
	else
	{
		debug(DEBUG_ERROR, "p_dsp->valuestring[%s]", p_dsp->valuestring);
		return FAILURE;
	}
}

int local_find_device(unsigned short profile_id, unsigned short device_id)
{
	int i = 0;
	unsigned short *profile = NULL;

	if (ZCL_HA_PROFILE_ID == profile_id)
		profile = HA_PROFILE_DEVICE;
	else if (ZLL_PROFILE_ID == profile_id)
		profile = ZLL_PROFILE_DEVICE;
	else if (ZCL_SHUNCOM_PROFILE_ID == profile_id)
		profile = SHUNCOM_PROFILE_ID;
	else
	{
		debug(DEBUG_ERROR, "couldn't find right profile id,profile id error[%d]", profile_id);
		return FAILURE;
	}

	while (profile[i] != 0xffff)
	{
		if (device_id == profile[i])
			return SUCCESS;
		else
			i = i + 1;
	}

	return FAILURE;
}

cluaster_info *local_find_cluster(unsigned short cluster_id, cluaster_info *cluasters)
{
	int i = 0;

	if (cluasters == NULL)
		return (cluaster_info *)NULL;
	while (cluasters[i].attributes != NULL)
	{
		//debug(DEBUG_ERROR,"cluster_id[%d] destcluster_id[%d]",cluster_id,cluasters[i].cluaster_id);
		if (cluster_id == cluasters[i].cluaster_id)
			return (&(cluasters[i]));
		i = i + 1;
	}

	return (cluaster_info *)NULL;
}

attribute_info *local_find_attribute(unsigned short attribute_id, cluaster_info *cluster)
{
	int i = 0;

	if (cluster == NULL)
		return (attribute_info *)NULL;

	while (strcmp(cluster->attributes[i].key, END_FLAG_BUF) != 0)
	{
		//debug(DEBUG_ERROR,"attribute_id[%d] dest_attribute_id[%d]",cluster->attributes[i].attribute_id,attribute_id);
		if (attribute_id == cluster->attributes[i].attribute_id)
			return (&(cluster->attributes[i]));
		i = i + 1;
	}

	return (attribute_info *)NULL;
}

int local_list_new(cJSON **r_result)
{
	device_info_t *device = NULL;
	device_info_t *pdevice = NULL;
	cluaster_info *cluaster = NULL;
	attribute_info *attribute = NULL;
	uint8_t ieeeAddr[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	char ieeeaddr_string[17] = {0};
	coordInfo_t coordInfo = {0};
	int i = 0;
	int j = 0;
	int k = 0;
	int finded = 0;

	cJSON *p_devices = cJSON_CreateArray();
	if (!p_devices)
	{
		debug(DEBUG_ERROR, "couldn't find device");
		*r_result = NULL;
		return CMD_MALLOC_ERROR;
	}

	cJSON *result = cJSON_CreateObject();
	if (!result)
	{
		cJSON_Delete(p_devices);
		p_devices = NULL;
		debug(DEBUG_ERROR, "couldn't find device");
		*r_result = NULL;
		return CMD_MALLOC_ERROR;
	}

	coord_getInfo(&coordInfo);

	avl_for_each_element_safe(&(device_table_head.hdr), device, avl, pdevice)
	{
		if ((memcmp(device->extAddr, coordInfo.ieeeAddr, 8) != 0) && (memcmp(ieeeAddr, device->extAddr, 8) != 0))
		{
			for (i = 0; i < device->numOfEps; i++)
			{
				cJSON *p_device = cJSON_CreateObject();
				if (!p_device)
				{
					debug(DEBUG_ERROR, "device create error");
					continue;
				}
#if 0
				if(FAILURE == local_find_device(device->epList[i].profileId,device->epList[i].deviceId))
				{
					cJSON_Delete(p_device);
					p_device = NULL;
					debug(DEBUG_ERROR,"device profile_id[%d] device_id[%d] error",device->epList[i].profileId,device->epList[i].deviceId);
					continue;
				}
#endif
				snprintf(ieeeaddr_string, 17, "%02x%02x%02x%02x%02x%02x%02x%02x",
						 device->extAddr[0], device->extAddr[1], device->extAddr[2],
						 device->extAddr[3], device->extAddr[4], device->extAddr[5],
						 device->extAddr[6], device->extAddr[7]);

				cJSON_AddStringToObject(p_device, "id", ieeeaddr_string);
				cJSON_AddBoolToObject(p_device, "ol", device->online);
				cJSON_AddNumberToObject(p_device, "ep", device->epList[i].endpointId);
				cJSON_AddNumberToObject(p_device, "pid", device->epList[i].profileId);
				cJSON_AddNumberToObject(p_device, "did", device->epList[i].deviceId);

				for (j = 0; j < device->epList[i].num_op_clusters; j++)
				{
					cluaster = NULL;
					cluaster = local_find_cluster(device->epList[i].op_cluster_list[j].cluster_id, cluasters);
					if (cluaster == NULL)
					{
						debug(DEBUG_ERROR, "conn't find cluster_id[%d]", device->epList[i].op_cluster_list[j].cluster_id);
						continue;
					}
					debug(DEBUG_ERROR, "cluster_id[%d]", device->epList[i].op_cluster_list[j].cluster_id);
					for (k = 0; k < device->epList[i].op_cluster_list[j].num_attributes; k++)
					{
						attribute = NULL;
						attribute = local_find_attribute(device->epList[i].op_cluster_list[j].attribute_list[k].attr_id, cluaster);
						if (attribute == NULL)
						{
							debug(DEBUG_ERROR, "conn't find attribute_id[%d]", device->epList[i].op_cluster_list[j].attribute_list[k].attr_id);
							continue;
						}
						debug(DEBUG_ERROR, "attribute_id[%d]", device->epList[i].op_cluster_list[j].attribute_list[k].attr_id);
						debug(DEBUG_ERROR, "attribute->visible[%d]", attribute->visible);
						if (attribute->visible != VISIBLE)
						{
							debug(DEBUG_ERROR, "attribute_id[%d] is disvisible", device->epList[i].op_cluster_list[j].attribute_list[k].attr_id);
							continue;
						}
						//debug(DEBUG_ERROR,"attribute->key[%s]",attribute->key);
						if (attribute->data_type == TYPE_STRING)
							cJSON_AddStringToObject(p_device, attribute->key, device->epList[i].op_cluster_list[j].attribute_list[k].attr_val + 1);
						else if (attribute->data_type == TYPE_BOOL)
							cJSON_AddBoolToObject(p_device, attribute->key, device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[0]);
						else if (attribute->data_type == TYPE_BYTE)
							cJSON_AddNumberToObject(p_device, attribute->key, device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[0]);
						else if (attribute->data_type == TYPE_SHORT_STRING)
						{
							short temp = (short)((((unsigned short)device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[1] & 0x00ff) << 8) + (unsigned short)device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[0]);
							unsigned char buf_temp[10] = {0};
							sprintf(buf_temp, "%d.%u%u", temp / 100, (temp % 100) / 10, temp % 10);
							cJSON_AddStringToObject(p_device, attribute->key, buf_temp);
						}
						else if (attribute->data_type == TYPE_SHORT)
						{
							unsigned short temp = (((unsigned short)device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[1] & 0x00ff) << 8) + (unsigned short)device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[0];
							if ((attribute->attribute_id == ATTRID_HVAC_THERMOSTAT_OCCUPIED_HEATING_SETPOINT) && (cluaster->cluaster_id == ZCL_CLUSTER_ID_HVAC_THERMOSTAT))
							{
								temp = temp / 100;
							}
							cJSON_AddNumberToObject(p_device, attribute->key, temp);
						}
						else if (attribute->data_type == TYPE_THREE_STRING)
						{
							unsigned int temp = (((unsigned int)device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[2] & 0x000000ff) << 16) + (((unsigned int)device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[1] & 0x000000ff) << 8) + (unsigned int)device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[0];
							unsigned char buf_temp[30] = {0};
							sprintf(buf_temp, "%d", temp);
							cJSON_AddStringToObject(p_device, attribute->key, buf_temp);
						}
						else if (attribute->data_type == TYPE_THREE)
						{
							unsigned int temp = (((unsigned int)device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[2] & 0x000000ff) << 16) + (((unsigned int)device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[1] & 0x000000ff) << 8) + (unsigned int)device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[0];
							cJSON_AddNumberToObject(p_device, attribute->key, temp);
						}
						else if (attribute->data_type == TYPE_INT_STRING)
						{
							unsigned int temp = (((unsigned int)device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[3] & 0x000000ff) << 24) + (((unsigned int)device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[2] & 0x000000ff) << 16) + (((unsigned int)device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[1] & 0x000000ff) << 8) + (unsigned int)device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[0];
							unsigned char buf_temp[30] = {0};
							sprintf(buf_temp, "%d", temp);
							cJSON_AddStringToObject(p_device, attribute->key, buf_temp);
						}
						else if (attribute->data_type == TYPE_INT)
						{
							unsigned int temp = (((unsigned int)device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[3] & 0x000000ff) << 24) + (((unsigned int)device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[2] & 0x000000ff) << 16) + (((unsigned int)device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[1] & 0x000000ff) << 8) + (unsigned int)device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[0];
							cJSON_AddNumberToObject(p_device, attribute->key, temp);
						}
						else if (attribute->data_type == TYPE_LL_STRING)
						{
							unsigned long long temp = (((unsigned long long)device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[0] & 0x00000000000000ff) << 0) + (((unsigned long long)device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[1] & 0x00000000000000ff) << 8) + (((unsigned long long)device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[2] & 0x00000000000000ff) << 16) + (((unsigned long long)device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[3] & 0x00000000000000ff) << 24) + (((unsigned long long)device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[4] & 0x00000000000000ff) << 32) + (((unsigned long long)device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[5] & 0x00000000000000ff) << 40) + (((unsigned long long)device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[6] & 0x00000000000000ff) << 48) + (((unsigned long long)device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[7] & 0x00000000000000ff) << 56);
							unsigned char buf_temp[40] = {0};
							sprintf(buf_temp, "%d", temp);
							cJSON_AddStringToObject(p_device, attribute->key, buf_temp);
						}
						else if (attribute->data_type == TYPE_LL)
						{
							unsigned long long temp = (((unsigned long long)device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[0] & 0x00000000000000ff) << 0) + (((unsigned long long)device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[1] & 0x00000000000000ff) << 8) + (((unsigned long long)device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[2] & 0x00000000000000ff) << 16) + (((unsigned long long)device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[3] & 0x00000000000000ff) << 24) + (((unsigned long long)device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[4] & 0x00000000000000ff) << 32) + (((unsigned long long)device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[5] & 0x00000000000000ff) << 40) + (((unsigned long long)device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[6] & 0x00000000000000ff) << 48) + (((unsigned long long)device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[7] & 0x00000000000000ff) << 56);
							cJSON_AddNumberToObject(p_device, attribute->key, temp);
						}
						else
						{
							debug(DEBUG_ERROR, "attribute date type error[%d]", attribute->data_type);
							continue;
						}
					}
				}
				if ((FAILURE == local_find_device(device->epList[i].profileId, device->epList[i].deviceId)) && (FAILURE == local_is_lvmi_device(p_device)))
				{
					cJSON_Delete(p_device);
					p_device = NULL;
					debug(DEBUG_ERROR, "device profile_id[%d] device_id[%d] error", device->epList[i].profileId, device->epList[i].deviceId);
					continue;
				}
				cJSON_AddItemToArray(p_devices, p_device);
			}
			finded = 1;
		}
		debug(DEBUG_DEBUG, "********* next device *********");
	}

	/*if(finded != 1)
	{
		cJSON_Delete(p_devices);
		p_devices = NULL;
		cJSON_Delete(result);
		result = NULL;
		debug(DEBUG_ERROR,"couldn't find device");
		*r_result =  NULL;
		return CMD_FAILURE;
	}*/

	cJSON_AddItemToObject(result, "devices", p_devices);
	*r_result = result;
	return CMD_SUCCESS;
}
//通知用户：当zigbee网关下的节点状态发生改变时，SDK会进入该回掉并打印状态改变的那个zigbee节点最新状态，在终端打印
Zstatus_t deviceStateChange(deviceInfo_t *deviceInfo, uint16_t clusterId)
{
	cJSON *r_result = NULL;
	char *send_data = NULL;
	local_list_new(&r_result);
	send_data = cJSON_Print(r_result);
	if (send_data != NULL)
	{
		debug(DEBUG_DEBUG, "device info[%s]", send_data);
		cJSON_Delete(r_result);
		r_result = NULL;
		free(send_data);
		send_data = NULL;
	}
	else
	{
		cJSON_Delete(r_result);
		r_result = NULL;
	}
	return ZSUCCESS;
}
//有设备向网关注销
int device_unregister(unsigned char ieee_addr[8])
{
	char ieee[17] = {0};

	sprintf(ieee, "%02x%02x%02x%02x%02x%02x%02x%02x",
			ieee_addr[0], ieee_addr[1], ieee_addr[2], ieee_addr[3],
			ieee_addr[4], ieee_addr[5], ieee_addr[6], ieee_addr[7]);

	debug(DEBUG_ERROR, "device leave ieee[%s]", ieee);
}

void *test(void *arg)
{
	return;
}
//通知用户：sdk和网关模块初始化状态已经完成，当SDK进入这个回调时zigbee网关部分初始化完成，可对节点进行操作
Zstatus_t zigbeeModuleStatus(ZigbeeModuleStatus_t status)
{
	unsigned char tmp_ieee[8] = {0};
	memset(tmp_ieee, 0, 8);
	device_setPermitJoin(tmp_ieee, 0);
	memset(tmp_ieee, 0xff, 8);
	device_setPermitJoin(tmp_ieee, 0);
}
//组的状态发生改变
int zigbeeGroupStateChange(uint8_t ieee_Addr[8], uint8_t epId, uint8_t cmdId, uint8_t status, uint8_t grpCnt, uint16_t *grpList, uint8_t capacity, char *grpName)
{
	if (cmdId == 0x00) //group add rsp
	{
		debug(DEBUG_ERROR, "group add rsp");
	}
	else if (cmdId == 0x01) //group view
	{
		debug(DEBUG_ERROR, "group view");
	}
	else if (cmdId == 0x02) //group member ship
	{
		debug(DEBUG_ERROR, "group member ship");
	}
	else if (cmdId == 0x03) //group remove rsp
	{
		debug(DEBUG_ERROR, "group remove rsp");
	}
	else //unknow cmd
	{
		debug(DEBUG_ERROR, "unknow cmd");
	}
	if ((cmdId == 0x00) || (cmdId == 0x03)) //group add rsp
	{
		debug(DEBUG_ERROR, "grpList[0][%d]", grpList[0]);
		debug(DEBUG_ERROR, "epId[%d]", epId);
	}
	else if ((cmdId == 0x02) || (cmdId == 0x01))
	{
	}
}
//场景改变会进入该会掉
int zigbeeSceneStateChange(uint8_t ieee_Addr[8], uint8_t epId, uint8_t cmdId, uint8_t status, uint8_t grpCnt, uint16_t *grpList, uint8_t capacity, char *grpName)
{
}
//如果设备从在线变成离线，或者从离线变成在线，会进入回掉
int device_online_offline_cb(unsigned char ieee_addr[8], char online_or_not)
{
	uint8 ieeeaddr_string[18] = {0};

	snprintf(ieeeaddr_string, 17, "%02x%02x%02x%02x%02x%02x%02x%02x",
			 ieee_addr[0], ieee_addr[1], ieee_addr[2], ieee_addr[3],
			 ieee_addr[4], ieee_addr[5], ieee_addr[6], ieee_addr[7]);

	debug(DEBUG_ERROR, "ieee[%s] is online_or_not[%d]", ieeeaddr_string, online_or_not);

	return 0;
}
//有设备发送命令数据，会进入该回掉
int pfn_zigbee_cmdIncoming(unsigned char ieee_addr[8], uint8_t epId, uint16_t clusterId, uint8_t cmdId, void *pCmd)
{
	uint8 *p_data = NULL;
	uint8 ieeeaddr_string[18] = {0};

	if (!pCmd)
		debug(DEBUG_ERROR, "pCmd is NULL");

	snprintf(ieeeaddr_string, 17, "%02x%02x%02x%02x%02x%02x%02x%02x",
			 ieee_addr[0], ieee_addr[1], ieee_addr[2], ieee_addr[3],
			 ieee_addr[4], ieee_addr[5], ieee_addr[6], ieee_addr[7]);

	debug(DEBUG_ERROR, "ieee[%s]", ieeeaddr_string);
	debug(DEBUG_ERROR, "epId[%d]", epId);
	debug(DEBUG_ERROR, "clusterId[%04x]", clusterId);
	debug(DEBUG_ERROR, "cmdId[%02x]", cmdId);

	switch (clusterId)
	{
	case ZCL_CLUSTER_ID_CLOSURES_DOOR_LOCK:
	{
		switch (cmdId)
		{
		case 0x21:
		{
			zclDoorLockProgrammingEventNotification_t *notify_program = (zclDoorLockProgrammingEventNotification_t *)pCmd;

			p_data = (uint8 *)pCmd + 11;

			debug(DEBUG_ERROR, "programEventSource[%d]", notify_program->programEventSource);
			debug(DEBUG_ERROR, "programEventCode[%d]", notify_program->programEventCode);
			debug(DEBUG_ERROR, "userID[%d]", notify_program->userID);
			debug(DEBUG_ERROR, "pin[%d]", notify_program->pin);
			debug(DEBUG_ERROR, "userType[%d]", notify_program->userType);
			debug(DEBUG_ERROR, "userStatus[%d]", notify_program->userStatus);
			debug(DEBUG_ERROR, "zigBeeLocalTime[%d]", notify_program->zigBeeLocalTime);
		}
		break;
		case 0x20:
		{
			zclDoorLockOperationEventNotification_t *notify_operation = (zclDoorLockOperationEventNotification_t *)pCmd;

			p_data = (uint8 *)pCmd + 9;

			debug(DEBUG_ERROR, "programEventSource[%d]", notify_operation->operationEventSource);
			debug(DEBUG_ERROR, "programEventCode[%d]", notify_operation->operationEventCode);
			debug(DEBUG_ERROR, "userID[%d]", notify_operation->userID);
			debug(DEBUG_ERROR, "pin[%d]", notify_operation->pin);
			debug(DEBUG_ERROR, "zigBeeLocalTime[%d]", notify_operation->zigBeeLocalTime);
		}
		break;
		default:
			debug(DEBUG_ERROR, "cmdId[%02x] is not support", cmdId);
			break;
		}
	}
	break;
	case ZCL_CLUSTER_ID_SS_IAS_ACE:
	{
		switch (cmdId)
		{
		case 0x04:
		{
			zclACEPanelStatusChanged_t *panel_cmd = (zclACEPanelStatusChanged_t *)pCmd;

			debug(DEBUG_ERROR, "panelStatus[%d]", panel_cmd->panelStatus);
			debug(DEBUG_ERROR, "secondsRemaining[%d]", panel_cmd->secondsRemaining);
			debug(DEBUG_ERROR, "audibleNotification[%d]", panel_cmd->audibleNotification);
			debug(DEBUG_ERROR, "alarmStatus[%d]", panel_cmd->alarmStatus);
		}
		break;
		default:
			debug(DEBUG_ERROR, "cmdId[%02x] is not support", cmdId);
			break;
		}
	}
	break;
	default:
		debug(DEBUG_ERROR, "clusterId[%04x] is not support", clusterId);
		break;
	}
}
//设备上报心跳信息，会进入该回掉
int user_zigbee_reportIn(device_info_t *device, uint8_t epId, uint16_t clusterId, deviceReportCmd_t *deviceReport)
{

	debug(DEBUG_ERROR, "*******enter heartbeat **************************************\n");
	uint8 ieeeaddr_string[18] = {0};
	uint8 ieee_addr[8] = {0};
	memcpy(ieee_addr, device->extAddr, 8);

	snprintf(ieeeaddr_string, 17, "%02x%02x%02x%02x%02x%02x%02x%02x",
			 ieee_addr[0], ieee_addr[1], ieee_addr[2], ieee_addr[3],
			 ieee_addr[4], ieee_addr[5], ieee_addr[6], ieee_addr[7]);

	debug(DEBUG_ERROR, "heartbeat device is *********************%s*****************\n", ieeeaddr_string);

	return CMD_SUCCESS;
}

user_zigbeeCBs_t zigbeeCBs = {
	zha_list_backinfo, //通知用户：获取网关下面所有zigbee节点的状态,可自己修改该函数，把节点信息读出来
	deviceStateChange,//通知用户：当zigbee网关下的节点状态发生改变时，SDK会进入该回掉并打印状态改变的那个zigbee节点最新状态，在终端打印
	zigbeeModuleStatus,//通知用户：sdk和网关模块初始化状态已经完成，当SDK进入这个回调时zigbee网关部分初始化完成，可对节点进行操作
	device_online_offline_cb,//如果设备从在线变成离线，或者从离线变成在线，会进入回掉
	NULL,//有设备向网关注册
	device_unregister,//有设备向网关注销
	zigbeeGroupStateChange,//组的状态发生改变
	zigbeeSceneStateChange,//场景改变会进入该会掉
	NULL,					
	pfn_zigbee_cmdIncoming,//有设备发送命令数据，会进入该回掉
	NULL,//有设备的属性发生改变					
	NULL,//有设备的cmd发生改变					
	user_zigbee_reportIn,//设备上报心跳信息，会进入该回掉
	NULL//网关发送结果
	};

void write_device_in_white_list(void)
{
	int i = 0;
	int j = 0;
	int find_flag = 0;
	uint16_t numOfDevices = 0;
	uint16_t num_ieeeAddr = 0;
	uint8_t ieeeAddr[200 * 8] = {0}; //200个64位
	char ieeeaddr_string[17] = {0};
	deviceInfo_t *devices = NULL;
	//实时获取当前网关下所有节点的(64位ieee)，返回设备个数
	numOfDevices = device_getDevicesList_fill(&devices);
	//从数据库中获取当前网网关下公共的白名单列表，返回白名单节点个数，保存设备64位ieee地址到数组中
	num_ieeeAddr = device_getWhitelist(ieeeAddr);
	//每个节点遍历公共的白名单列表，看该在不在白名单列表里
	for (i = 0; i < numOfDevices; i++) //遍历节点
	{
		for (j = 0; j < num_ieeeAddr; j++) //遍历公共白名单列表
		{
			if (memcmp(devices[i].deviceBasic.ieeeAddr, ieeeAddr + j * 8, 8) == 0) //两个指针同时向右移动64位
			{
				find_flag = 1; //该节点在白名单里
				break;
			}
		}

		snprintf(ieeeaddr_string, 17, "%02x%02x%02x%02x%02x%02x%02x%02x",
				 devices[i].deviceBasic.ieeeAddr[0], devices[i].deviceBasic.ieeeAddr[1],
				 devices[i].deviceBasic.ieeeAddr[2], devices[i].deviceBasic.ieeeAddr[3],
				 devices[i].deviceBasic.ieeeAddr[4], devices[i].deviceBasic.ieeeAddr[5],
				 devices[i].deviceBasic.ieeeAddr[6], devices[i].deviceBasic.ieeeAddr[7]);
		debug(DEBUG_ERROR, "device[%s]", ieeeaddr_string);

		if (find_flag == 0) //该节点不在白名单里
		{					//把该节点添加到白名单列表里
			if (ZSUCCESS != device_writeWhitelist(devices[i].deviceBasic.ieeeAddr))
			{
				debug(DEBUG_ERROR, "write device[%s] in whitelist failure", ieeeaddr_string);
			}
			else
				debug(DEBUG_ERROR, "write device[%s] in whitelist success", ieeeaddr_string);
		}
		else
		{
			debug(DEBUG_ERROR, "device[%s] is in white list", ieeeaddr_string);
			find_flag = 0; //下一个节点
		}
	}
}
//检查协调器信息是否正常
int cloud_is_coordinfo_ok(void)
{
	coordInfo_t coordInfo;
	coord_getInfo(&coordInfo);//获取协调其信息
	char wrong_ieee[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	//检查
	if (memcmp(coordInfo.ieeeAddr, wrong_ieee, 8) == 0)
	{
		debug(DEBUG_ERROR, "coordination ieee is wrong");
		return FAILURE;
	}
	else
	{
		//SCprintf("coordination ieee is right");
		return SUCCESS;
	}
}
//管理白名单线程，一直开
void *cloud_thread(void *p)
{
	int step = 1;
	//等待协调器信息正常
	while (FAILURE == cloud_is_coordinfo_ok())
	{
		sleep(2);
	}
	//首先关闭白名单列表
	//然后开放所有zigbee节点许可60秒permitjoin
	//在这60秒里不停的把当前网关下所有节点加入到白名单中
	//60秒后打开白名单列表，最后退出循环
	while (1)
	{
		sleep(1);

		switch (step)
		{
		case 1: //disable whitelist
		{
			device_enableWhitelist(0); //关闭白名单列表
			debug(DEBUG_DEBUG, "disable whitelist");
			break;
		}
		case 2: //open permit join ,240s
		{
			uint8_t ieeeaddr[8] = {0};
			memset(ieeeaddr, 0xff, 8);			//所有zigbee地址1111 1111,[8]
			device_setPermitJoin(ieeeaddr, 60); //一分钟
			debug(DEBUG_DEBUG, "open permit join ,60s");
			break;
		}
		case 60: //enable whitelist
		{
			device_enableWhitelist(1); //使能白名单列表
			step = 0;
			debug(DEBUG_DEBUG, "enable whitelist");
			return NULL;
			break;
		}
		default:
		{//60秒内，把当前网关下的所有节点加到白名单里
			write_device_in_white_list();

			break;
		}
		}
		step++;
	}
}//这个线程相当于自己return了

//ZHA:ZigBee Home Automation,zigbee智能家居的标准
int main(int argc, char *argv[])
{
	printf("*********************************************************\n");
	printf("************sdk version: [%s]**********\n", lib_compileTime());
	printf("*********************************************************\n");
	uloop_init();//使用库初始化uloop
	if (argc != 2)
	{
		debug(DEBUG_GENERAL, "argv is error");
		return -1;
	}
	zigbeezap(argv[1]);
	zigbee_init("/etc/config/zha.db");
	ubus_init();//创建epoll句柄，最多监听32个fd
	user_registerZigbeeCBs(&zigbeeCBs);//注册回掉函数

	// device_loadManufacturerSupperversion("Simon","SZSiMon WallSwitch1",240);
	device_loadManufacturerSupperversion("Simon", "SZSiMon WallSwitch2", 240);
	device_loadManufacturerSupperversion("Simon", "SZSiMon WallSwitch3", 240);
	device_loadManufacturerSupperversion("Simon", "SZSiMon WallSwitch4", 240);
	device_loadManufacturerSupperversion("Simon", "SZSiMon SceneSwitch6", 240);
	//设备制造商信息
	device_loadManufacturerSupperversion("philips", "LCT007", 0xfffe);

#if 1

	//device_loadManufacturerSupperversion("LUMI","lumi.plug",60);

	pthread_t cloud_thread_t;
	pthread_t mqtt_thread_t;
	//管理白名单
	if (pthread_create(&cloud_thread_t, NULL, cloud_thread, (void *)NULL) != 0)
	{
		debug(DEBUG_ERROR, " pthread of cloud thread creates error:%s ", strerror(errno));
	}
	else
		debug(DEBUG_DEBUG, " pthread of cloud thread creates successfully ");

#if 1
	//mqtt
	if (pthread_create(&mqtt_thread_t, NULL, mqtt_thread, (void *)NULL) != 0)
	{
		debug(DEBUG_ERROR, " pthread of cloud thread creates error:%s ", strerror(errno));
	}
	else
		debug(DEBUG_DEBUG, " pthread of cloud thread creates successfully ");

#endif

#endif
	debug(DEBUG_GENERAL, "uloop run");
#if 1 //生成MD5
	unsigned char md[100] = {0};
	unsigned char d[11] = "1234567891";
	int i = 0;
	unsigned char *p = MD5((const unsigned char *)&d[0], strlen(d), (unsigned char *)&md[0]);//生成MD5
	if (p)
	{
		debug(DEBUG_ERROR, "p:");
		for (i = 0; i < strlen(p); i++)
		{
			if (i % 10 == 0)
				printf("\n");
			printf("%02x", (unsigned char)p[i]);
		}
		printf("\n");
	}
	else
	{
		debug(DEBUG_ERROR, "MD5 error");
	}

#endif
	//打开白名单列表
	device_enableWhitelist(1);
	zigbee_loop();//事件循环主入口
	return 0;
}
