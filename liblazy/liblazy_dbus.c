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
#include <errno.h>
#include <stdlib.h>
#include <string.h>

int liblazy_dbus_send_method_call(const char *destination, const char *path,
				  const char *interface, const char *method,
				  int bus_type,
				  DBusMessage **reply,
				  int first_arg_type, va_list var_args)
{
	DBusError	dbus_error;
	DBusConnection	*dbus_connection	= NULL;
	DBusMessage	*message		= NULL;
	int		ret			= 0;

	if (path == NULL || method == NULL)
		return LIBLAZY_ERROR_INVALID_ARGUMENT;
	
	dbus_error_init(&dbus_error);

	dbus_connection = dbus_bus_get(bus_type, &dbus_error);
	if (dbus_connection == NULL || dbus_error_is_set(&dbus_error)) {
		ERROR("Connection to dbus not ready, skipping method call %s: %s",
		      method, dbus_error.message);
		ret = LIBLAZY_ERROR_DBUS_NOT_READY;
		goto Free_Error;
	}

	message = dbus_message_new_method_call(destination, path, interface, method);
	dbus_message_append_args_valist(message, first_arg_type, var_args);

	if (reply == NULL) {
		if (!dbus_connection_send(dbus_connection, message, NULL)) {
			ERROR("Could not send method call: OOM");
			ret = LIBLAZY_ERROR_GENERAL;
		}
	} else {
		*reply = dbus_connection_send_with_reply_and_block(dbus_connection,
								   message, -1,
								   &dbus_error);
		if (dbus_error_is_set(&dbus_error)) {
			ERROR("Received error reply: %s", dbus_error.message);
			ret = LIBLAZY_ERROR_DBUS_ERROR_IS_SET;
		}
	}

	dbus_message_unref(message);
Free_Error:
	dbus_error_free(&dbus_error);
	return ret;
}

int liblazy_dbus_system_send_method_call(const char *destination, const char *path,
					 const char *interface, const char *method,
					 DBusMessage **reply,
					 int first_arg_type, ...)
{
	int	ret;
	va_list	var_args;

	va_start(var_args, first_arg_type);
	ret = liblazy_dbus_send_method_call(destination,
					    path,
					    interface,
					    method,
					    DBUS_BUS_SYSTEM,
					    reply,
					    first_arg_type,
					    var_args);
	va_end(var_args);
	return ret;
}

int liblazy_dbus_session_send_method_call(const char *destination, const char *path,
					  const char *interface, const char *method,
					  DBusMessage **reply,
					  int first_arg_type, ...)
{
	int	ret;
	va_list	var_args;

	va_start(var_args, first_arg_type);
	ret = liblazy_dbus_send_method_call(destination,
					    path,
					    interface,
					    method,
					    DBUS_BUS_SESSION,
					    reply,
					    first_arg_type,
					    var_args);
	va_end(var_args);
	return ret;
}

static int liblazy_dbus_send_signal(const char *path, const char *interface,
				    const char *name, int bus_type,
				    int first_arg_type, va_list var_args)
{
	DBusError	dbus_error;
	DBusMessage	*message;
	DBusConnection	*dbus_connection;
	int		ret = 0;

	if (path == NULL || interface == NULL || name == NULL)
		return LIBLAZY_ERROR_INVALID_ARGUMENT;
	
	dbus_error_init(&dbus_error);

	dbus_connection = dbus_bus_get(bus_type, &dbus_error);
	if (dbus_connection == NULL || dbus_error_is_set(&dbus_error)) {
		ERROR("Connection to dbus not ready, skipping signal %s: %s",
		      name, dbus_error.message);
		ret = LIBLAZY_ERROR_DBUS_NOT_READY;
		goto Free_Error;
	}

	message = dbus_message_new_signal(path, interface, name);
	dbus_message_append_args_valist(message, first_arg_type, var_args);

	if (!dbus_connection_send(dbus_connection, message, NULL)) {
		ERROR("Could not send signal to D-BUS system bus.");
		ret = LIBLAZY_ERROR_GENERAL;
	}

	dbus_message_unref(message);
Free_Error:
	dbus_error_free(&dbus_error);
	return ret;
}

int liblazy_dbus_system_send_signal(const char *path, const char *interface,
				    const char *name, int first_arg_type, ...)
{
	int	ret;
	va_list	var_args;

	va_start(var_args, first_arg_type);
	ret = liblazy_dbus_send_signal(path, interface, name, DBUS_BUS_SYSTEM,
				       first_arg_type, var_args);
	va_end(var_args);
	return ret;
}

int liblazy_dbus_session_send_signal(const char *path, const char *interface,
				     const char *name, int first_arg_type, ...)
{
	int	ret;
	va_list	var_args;

	va_start(var_args, first_arg_type);
	ret = liblazy_dbus_send_signal(path, interface, name, DBUS_BUS_SESSION,
				       first_arg_type, var_args);
	va_end(var_args);
	return ret;

}

int liblazy_dbus_message_get_basic_arg(DBusMessage *message, int type,
				       void *arg, int no)
{
	int		current_type;
	DBusMessageIter	iter;
	int		x		= 0;
	int		ret		= LIBLAZY_ERROR_GENERAL;
	int		_no		= 0;

	if (message == NULL)
		return LIBLAZY_ERROR_INVALID_ARGUMENT;

	for (dbus_message_iter_init(message, &iter);
	     (current_type = dbus_message_iter_get_arg_type(&iter)) != DBUS_TYPE_INVALID;
	     x++, dbus_message_iter_next(&iter)) {
		if (current_type == type) {
			if (_no < no) {
				_no++;
				continue;
			} else if (_no > no)
				break;
			else {
				dbus_message_iter_get_basic(&iter, arg);
				ret = 0;
				break;
			}
		}
	}

	return ret;
}

static char **liblazy_dbus_get_strlist_from_array(DBusMessageIter *reply_iter)
{
	const char	*val;
	DBusMessageIter	iter_array;
	char		**strlist	= NULL;
	int		i		= 0;

	if (dbus_message_iter_get_arg_type(reply_iter) != DBUS_TYPE_ARRAY) {
		ERROR("Iterator doesn't contain an array at current position");
		return NULL;
	}
	
	dbus_message_iter_recurse(reply_iter, &iter_array);

	strlist = (char **)malloc(0);
	while (dbus_message_iter_get_arg_type(&iter_array) == DBUS_TYPE_STRING) {
		dbus_message_iter_get_basic(&iter_array, &val);
		strlist = realloc(strlist, sizeof(char**) * (i + 1));
		strlist[i] = strdup(val);
		dbus_message_iter_next(&iter_array);
		i++;
	}
	strlist = realloc(strlist, sizeof(char**) * (i + 1));
	strlist[i] = NULL;
	return strlist;

}

int liblazy_dbus_message_get_strlist_arg(DBusMessage *message,
					 char ***strlist, int no)
{
	int		current_type;
	DBusMessageIter	iter;
	int		x		= 0;
	int		ret		= LIBLAZY_ERROR_GENERAL;
	int		_no		= 0;

	if (message == NULL)
		return LIBLAZY_ERROR_INVALID_ARGUMENT;

	for (dbus_message_iter_init(message, &iter);
	     (current_type = dbus_message_iter_get_arg_type(&iter)) != DBUS_TYPE_INVALID;
	     x++, dbus_message_iter_next(&iter)) {
		if (current_type == DBUS_TYPE_ARRAY) {
			if (_no < no) {
				_no++;
				continue;
			} else if (_no > no)
				break;
			else {
				*strlist = liblazy_dbus_get_strlist_from_array(&iter);
				ret = 0;
				break;
			}
		}
	}
	return ret;
}
