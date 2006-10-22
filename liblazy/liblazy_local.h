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

#ifndef LIBLAZY_LOCAL_H
#define LIBLAZY_LOCAL_H

#define ERROR(string, args...) \
do{\
	fprintf(stderr, "liblazy (%s:%d): "string"\n", __FUNCTION__, __LINE__, ## args); \
}while(0);

int liblazy_dbus_send_method_call(const char *destination, const char *path,
				  const char *interface, const char *method,
				  int bus_type,
				  DBusMessage **reply,
				  int first_arg_type, va_list var_args);

#endif /* LIBLAZY_LOCAL_H */
