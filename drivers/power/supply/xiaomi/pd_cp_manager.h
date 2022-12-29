
#include <linux/device.h>
#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/workqueue.h>
#include <linux/power_supply.h>

#define PDM_SM_DELAY_300MS	300
#define PDM_SM_DELAY_200MS	200
#define PDM_SM_DELAY_150MS	150
#define PDM_SM_DELAY_100MS	100

#define LARGE_IBAT_DIFF		650
#define MEDIUM_IBAT_DIFF	150
#define LARGE_VBAT_DIFF		400
#define MEDIUM_VBAT_DIFF	200
#define LARGE_IBUS_DIFF		500
#define MEDIUM_IBUS_DIFF	200
#define LARGE_VBUS_DIFF		800
#define MEDIUM_VBUS_DIFF	400
#ifdef CONFIG_FACTORY_BUILD
#define MAX_CABLE_RESISTANCE	550
#else
#define MAX_CABLE_RESISTANCE	350
#endif

#define LARGE_STEP		5
#define MEDIUM_STEP		2
#define SMALL_STEP		1

#define PDM_BBC_ICL		100
#define LOW_POWER_PD2_VINMIN 	4700
#define LOW_POWER_PD2_ICL 	1000

#define DEFAULT_PDO_VBUS_2S	10000
#define DEFAULT_PDO_IBUS_2S	3000
#define DEFAULT_PDO_VBUS_1S	5000
#define DEFAULT_PDO_IBUS_1S	3000

#define MIN_JEITA_CHG_INDEX	3
#define MAX_JEITA_CHG_INDEX	4
#define MIN_THERMAL_LIMIT_FCC	1500
#define LOW_THERMAL_LIMIT_FCC	2000
#define MIN_CP_IBUS		100
#define LOW_CP_IBUS		550

#define MAX_WATT_33W		34000000
#define MAX_WATT_67W		68000000
#define MAX_IBUS_67W		6200
#define SECOND_IBUS_67W		6100

#define STEP_MV			20

#define MAX_VBUS_TUNE_COUNT		40
#define MAX_ADAPTER_ADJUST_COUNT	10
#define MAX_ENABLE_CP_COUNT		5
#define MAX_TAPER_COUNT			5
#define MAX_LOW_CP_IBUS_COUNT		5

#define BYPASS_ENTRY_FCC	4000
#define BYPASS_EXIT_FCC		6000

#define BYPASS_MIN_VBUS_1S      3600
#define BYPASS_MIN_VBUS_2S      7200
