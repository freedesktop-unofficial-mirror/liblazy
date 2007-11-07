/***************************************************************************
 *                                                                         *
 *                              liblazy                                    *
 *                                                                         *
 *         Copyright (C) 2006,2007 Holger Macht <holger@homac.de>          *
 *                                                                         *
 *              Author(s): Holger Macht <holger@homac.de>                  *
 *                                                                         *
 * This library is free software; you can redistribute it and/or modify it *
 * under the terms of the GNU Lesser General Public License as published   *
 * by the Free Software Foundation; either version 2.1 of the License, or  *
 * (at your option) any later version.                                     *
 *                                                                         *
 * This library is distributed in the hope that it will be useful, but     *
 * WITHOUT ANY WARRANTY; without even the implied warranty of              *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU       *
 * Lesser General Public License for more details.                         *
 *                                                                         *
 * You should have received a copy of the GNU Lesser General Public        *
 * License along with this library; if not, write to the Free Software     *
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA           *
 * 02110-1301  USA                                                         *
 *                                                                         *
 ***************************************************************************/

#include "liblazy.h"
#include "liblazy_local.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define DBUS_HAL_SERVICE		"org.freedesktop.Hal"
#define DBUS_HAL_DEVICE_INTERFACE	"org.freedesktop.Hal.Device"
#define DBUS_HAL_MANAGER_PATH		"/org/freedesktop/Hal/Manager"
#define DBUS_HAL_MANAGER_INTERFACE	"org.freedesktop.Hal.Manager"
#define DBUS_HAL_COMPUTER_PATH		"/org/freedesktop/Hal/devices/computer"

static int liblazy_hal_property_exists(const char *udi, const char *property)
{
	int		error	= 0;
	int		exists	= 0;
	DBusMessage	*reply;

	if (udi == NULL || property == NULL)
		return LIBLAZY_ERROR_INVALID_ARGUMENT;

	error = liblazy_dbus_system_send_method_call(DBUS_HAL_SERVICE,
						     udi,
						     DBUS_HAL_DEVICE_INTERFACE,
						     "PropertyExists",
						     &reply,
						     DBUS_TYPE_STRING,
						     &property,
						     DBUS_TYPE_INVALID);

	if (error) {
		ERROR("Error checking if property '%s' exists", property);
		return error;
	}

	error = liblazy_dbus_message_get_basic_arg(reply, DBUS_TYPE_BOOLEAN,
						   &exists, 0);
	if (reply != NULL)
		dbus_message_unref(reply);
	if (exists == 1)
		return 1;
	return error;
}

static int liblazy_hal_get_property(const char *udi, const char *property,
				    const char *method, int type, void *value)
{
	int		error = 0;
	DBusMessage	*reply;

	if (udi == NULL || property == NULL )
		return LIBLAZY_ERROR_INVALID_ARGUMENT;

	error = liblazy_dbus_system_send_method_call(DBUS_HAL_SERVICE,
						     udi,
						     DBUS_HAL_DEVICE_INTERFACE,
						     method,
						     &reply,
						     DBUS_TYPE_STRING,
						     &property,
						     DBUS_TYPE_INVALID);	

	if (error) {
		ERROR("Error sending '%s' to HAL", method);
		return error;
	}

	error = liblazy_dbus_message_get_basic_arg(reply, type, value, 0);
	if (reply != NULL)
		dbus_message_unref(reply);
	if (error)
		ERROR("Error fetching property '%s'", property);
	return error;
}

static int liblazy_hal_get_strlist_manager(char ***strlist, const char *method,
					   int first_arg_type, ...)
{
	int		error	= 0;
	DBusMessage	*reply;
	va_list		var_args;

	va_start(var_args, first_arg_type);

	error = liblazy_dbus_send_method_call(DBUS_HAL_SERVICE,
					      DBUS_HAL_MANAGER_PATH,
					      DBUS_HAL_MANAGER_INTERFACE,
					      method,
					      DBUS_BUS_SYSTEM,
					      &reply,
					      first_arg_type,
					      var_args);
	va_end(var_args);

	if (error) {
		ERROR("Error while sending method %s to HAL", method);
		return error;
	}

	error = liblazy_dbus_message_get_strlist_arg(reply, strlist, 0);
	if (reply != NULL)
		dbus_message_unref(reply);
	return error;
}

int liblazy_hal_get_property_string(const char *udi, const char *property,
				    char **value)
{
	char	*str;
	int	ret;

	ret = liblazy_hal_property_exists(udi, property);
	if (ret != 1) {
		if (ret == 0)
			ret = LIBLAZY_ERROR_HAL_NO_SUCH_PROPERTY;
		goto Error;
	}

	ret = liblazy_hal_get_property(udi, property, "GetPropertyString",
				       DBUS_TYPE_STRING, &str);
	if (ret)
		goto Error;

	*value = strdup(str);
	return ret;
Error:
	*value = NULL;
	return ret;
}

int liblazy_hal_get_property_int(const char *udi, const char *property,
				 int *value)
{
	int ret;

	ret = liblazy_hal_property_exists(udi, property);
	if (ret != 1) {
		if (ret == 0)
			ret = LIBLAZY_ERROR_HAL_NO_SUCH_PROPERTY;
		*value = -1;
		return ret;
	}

	return liblazy_hal_get_property(udi, property, "GetPropertyInteger",
					DBUS_TYPE_INT32, value);
}

int liblazy_hal_get_property_bool(const char *udi, const char *property,
				  int *value)
{
	int ret;

	ret = liblazy_hal_property_exists(udi, property);
	if (ret != 1) {
		if (ret == 0)
			ret = LIBLAZY_ERROR_HAL_NO_SUCH_PROPERTY;
		*value = -1;
		return ret;
	}

	return liblazy_hal_get_property(udi, property, "GetPropertyBoolean",
					DBUS_TYPE_BOOLEAN, value);
}

int liblazy_hal_get_property_strlist(const char *udi, const char *property,
				     char ***strlist)
{
	int		error	= 0;
	DBusMessage	*reply;

	if (udi == NULL || property == NULL )
		return LIBLAZY_ERROR_INVALID_ARGUMENT;

	error = liblazy_hal_property_exists(udi, property);
	if (error != 1) {
		if (error == 0)
			error = LIBLAZY_ERROR_HAL_NO_SUCH_PROPERTY;
		goto Error;
	}

	error = liblazy_dbus_system_send_method_call(DBUS_HAL_SERVICE,
						     udi,
						     DBUS_HAL_DEVICE_INTERFACE,
						     "GetPropertyStringList",
						     &reply,
						     DBUS_TYPE_STRING,
						     &property,
						     DBUS_TYPE_INVALID);	

	if (error) {
		ERROR("Error while getting strlist property %s from HAL", property);
		goto Error;
	}

	error = liblazy_dbus_message_get_strlist_arg(reply, strlist, 0);
	if (reply != NULL)
		dbus_message_unref(reply);
	if (error)
		goto Error;
	return error;
 
Error:
	strlist[0] = NULL;
	return error;
}

int liblazy_hal_query_capability(const char *udi, const char *capability)
{
	int	i;
	int	error	= -1;
	char	**caps	= NULL;

	if (udi == NULL || capability == NULL )
		return LIBLAZY_ERROR_INVALID_ARGUMENT;

	error = liblazy_hal_get_property_strlist(udi, "info.capabilities", &caps);
	if (error)
		return error;

	if (caps != NULL) {
		for (i = 0; caps[i] != NULL; i++) {
			if (strcmp(caps[i], capability) == 0)
				error = 1;
		}
		liblazy_free_strlist(caps);
	}

	return error;
}

int liblazy_hal_find_device_by_capability(const char *capability, char ***strlist)
{
	int error;
	error = liblazy_hal_get_strlist_manager(strlist, "FindDeviceByCapability",
						DBUS_TYPE_STRING, &capability,
						DBUS_TYPE_INVALID);
	if (error) {
		strlist[0] = NULL;
		*strlist = NULL;
	}
	return error;

}

int liblazy_hal_find_device_by_string_match(const char *key, const char *value,
					    char ***strlist)
{
	int error;
	error = liblazy_hal_get_strlist_manager(strlist, "FindDeviceStringMatch",
						DBUS_TYPE_STRING, &key,
						DBUS_TYPE_STRING, &value, DBUS_TYPE_INVALID);
	if (error) {
		strlist[0] = NULL;
		*strlist = NULL;
	}
	return error;
}

int liblazy_hal_is_caller_privileged(const char *privilege)
{
	DBusMessage	*reply;
	DBusError	dbus_error;
	DBusConnection	*dbus_connection;
	const char	*unique_name;
	char		*allowed;
	int		error = 0;

	if (privilege == NULL )
		return LIBLAZY_ERROR_INVALID_ARGUMENT;

	dbus_error_init(&dbus_error);

	dbus_connection = dbus_bus_get(DBUS_BUS_SYSTEM, &dbus_error);
	if (dbus_error_is_set(&dbus_error)) {
		ERROR("Connection to dbus not ready, skipping privilege "
		      "lookup for privilege %s: %s\n",
		      privilege, dbus_error.message);
		dbus_error_free(&dbus_error);
		return LIBLAZY_ERROR_DBUS_NOT_READY;
	}

	unique_name = dbus_bus_get_unique_name(dbus_connection);

	error = liblazy_dbus_system_send_method_call(DBUS_HAL_SERVICE,
						     DBUS_HAL_COMPUTER_PATH,
						     DBUS_HAL_DEVICE_INTERFACE,
						     "IsCallerPrivileged",
						     &reply,
						     DBUS_TYPE_STRING, &privilege, 
						     DBUS_TYPE_STRING, &unique_name, 
						     DBUS_TYPE_INVALID);

	if (error)
		return error;

	error = liblazy_dbus_message_get_basic_arg(reply, DBUS_TYPE_STRING,
						   &allowed, 0);

	if (reply != NULL)
		dbus_message_unref(reply);
	if (error)
		return error;

	if (strcmp(allowed, "yes") == 0)
		return 1;
	return 0;
}
