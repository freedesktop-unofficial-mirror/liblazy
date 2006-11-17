#include "liblazy.h"

#include <stdio.h>
#include <unistd.h>

#define DBUS_NM_SERVICE			"org.freedesktop.NetworkManager"
#define DBUS_NM_INTERFACE		"org.freedesktop.NetworkManager"
#define DBUS_NM_PATH			"/org/freedesktop/NetworkManager"
#define DBUS_HAL_MANAGER_INTERFACE	"org.freedesktop.Hal.Manager"
#define DBUS_HAL_DEVICE_INTERFACE	"org.freedesktop.Hal.Device"
#define DBUS_HAL_UDI_COMPUTER		"/org/freedesktop/Hal/devices/computer"
#define DBUS_HAL_INTERFACE		"org.freedesktop.Hal"
#define DBUS_HAL_CPUFREQ_INTERFACE	"org.freedesktop.Hal.Device.CPUFreq"
#define DBUS_SYSTEM_PM_INTERFACE	"org.freedesktop.Policy.Power"

int main(int argc, char *argv[])
{
	char *governor = "ondemand";
	char *rpstring = NULL;
	int intval = -1;

	int ret = 0;
	DBusMessage *reply = NULL;

	printf("Try to set the ondemand governor\n");
	ret = liblazy_dbus_system_send_method_call(DBUS_HAL_INTERFACE,
						   DBUS_HAL_UDI_COMPUTER,
						   DBUS_HAL_CPUFREQ_INTERFACE,
						   "SetCPUFreqGovernor",
						   &reply,
						   DBUS_TYPE_STRING,
						   &governor,
						   DBUS_TYPE_INVALID);
	printf("return value: %d\n", ret);
	if (!ret)
		dbus_message_unref(reply);

	printf("------------------\n");
	printf("Try to get the current governor\n");
	ret = liblazy_dbus_system_send_method_call(DBUS_HAL_INTERFACE,
						   DBUS_HAL_UDI_COMPUTER,
						   DBUS_HAL_CPUFREQ_INTERFACE,
						   "GetCPUFreqGovernor",
						   &reply,
						   DBUS_TYPE_INVALID);

	ret = liblazy_dbus_message_get_basic_arg(reply, DBUS_TYPE_STRING, &rpstring, 0);
	printf("returned string: %s\n", rpstring);
	printf("return value: %d\n", ret);
	if (!ret)
		dbus_message_unref(reply);

	printf("--------------------\n");
	ret = liblazy_hal_get_property_string("/org/freedesktop/Hal/devices/computer",
					      "system.kernel.name",
					      &rpstring);
	printf("return value: %d\n", ret);
	printf("returned string: %s\n", rpstring);

	printf("string: %s\n", rpstring);
	liblazy_free_string(rpstring);

	printf("--------------------\n");
	printf("Try to get a string property from HAL\n");
	ret = liblazy_hal_get_property_string("/org/freedesktop/Hal/devices/computer",
					      "system.kernel.name",
					      &rpstring);
	printf("return value: %d\n", ret);
	printf("returned string: %s\n", rpstring);
	liblazy_free_string(rpstring);

	printf("--------------------\n");
	printf("Try to get a boolean value from HAL\n");
	ret = liblazy_hal_get_property_bool("/org/freedesktop/Hal/devices/computer",
					      "power_management.can_hibernate",
					      &intval);
	printf("return value: %d\n", ret);
	printf("returned int: %d\n", intval);

	printf("--------------------\n");
	printf("Try to get a integer value from HAL\n");
	ret = liblazy_hal_get_property_int("/org/freedesktop/Hal/devices/acpi_BAT0",
					    "battery.charge_level.current",
					    &intval);
	printf("return value: %d\n", ret);
	printf("returned int: %d\n", intval);


	printf("--------------------\n");
	ret = liblazy_hal_get_property_string("/org/freedesktop/Hal/devices/computer",
					      "system.kernel.name",
					      &rpstring);
	printf("return value: %d\n", ret);
	printf("returned string: %s\n", rpstring);

	printf("string: %s\n", rpstring);
	liblazy_free_string(rpstring);

	printf("--------------------\n");
	printf("Try to get a string property from HAL\n");
	ret = liblazy_hal_get_property_string("/org/freedesktop/Hal/devices/computer",
					      "system.kernel.name",
					      &rpstring);
	printf("return value: %d\n", ret);
	printf("returned string: %s\n", rpstring);
	liblazy_free_string(rpstring);

	printf("--------------------\n");
	printf("Try to get a boolean value from HAL\n");
	ret = liblazy_hal_get_property_bool("/org/freedesktop/Hal/devices/computer",
					      "power_management.can_hibernate",
					      &intval);
	printf("return value: %d\n", ret);
	printf("returned int: %d\n", intval);

	printf("--------------------\n");
	printf("Trying to send signal over system bus\n");
	ret = liblazy_dbus_system_send_signal("/macht/holger/super",
					      "org.holger.super",
					      "heyho",
					      DBUS_TYPE_INVALID);
	printf("return value: %d\n", ret);
	

	return 0;
}
