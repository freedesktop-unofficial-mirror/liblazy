/***************************************************************************
 *                                                                         *
 *                              liblazy                                    *
 *                                                                         *
 *           Copyright (C) 2006 Holger Macht <holger@homac.de>             *
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

#include <stdio.h>
#include <stdlib.h>
#include <pwd.h>
#include <string.h>
#include <errno.h>

#define DBUS_HAL_SERVICE		"org.freedesktop.Hal"
#define DBUS_HAL_DEVICE_INTERFACE	"org.freedesktop.Hal.Device"
#define DBUS_HAL_COMPUTER_PATH		"/org/freedesktop/Hal/devices/computer"

int liblazy_polkit_is_user_allowed_by_name(char *user, char *privilege)
{
	DBusMessage	*reply;
	DBusError	dbus_error;
	DBusConnection	*dbus_connection;
	const char	*unique_name;
	char		*allowed;
	int		error = 0;

	if (user == NULL || privilege == NULL )
		return LIBLAZY_ERROR_INVALID_ARGUMENT;

	dbus_error_init(&dbus_error);

	dbus_connection = dbus_bus_get(DBUS_BUS_SYSTEM, &dbus_error);
	if (dbus_error_is_set(&dbus_error)) {
		ERROR("Connection to dbus not ready, skipping privilege "
		      "lookup for privilege %s for user %s: %s\n",
		      privilege, user, dbus_error.message);
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

int liblazy_polkit_is_user_allowed_by_uid(int uid, char *privilege)
{
	struct passwd *pw = getpwuid(uid);

	if (pw == NULL) {
		ERROR("Could not get current username: %s", strerror(errno));
		return LIBLAZY_ERROR_GENERAL;
	}

	return liblazy_polkit_is_user_allowed_by_name(pw->pw_name, privilege);
}

int liblazy_polkit_is_user_allowed(char *privilege)
{
	char *user = getenv("USER");
	return liblazy_polkit_is_user_allowed_by_name(user, privilege);
}

