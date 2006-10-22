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

#ifndef LIBLAZY_H
#define LIBLAZY_H

#define DBUS_API_SUBJECT_TO_CHANGE 1
#include <dbus/dbus.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** @defgroup liblazy Liblazy - D-Bus methods provided for convenience
 * @{ */

#define LIBLAZY_ERROR_GENERAL			-1
#define LIBLAZY_ERROR_HAL_NOT_READY		-10
#define LIBLAZY_ERROR_HAL_NO_SUCH_PROPERTY	-11

#define LIBLAZY_ERROR_DBUS_NOT_READY		-20
#define LIBLAZY_ERROR_DBUS_NO_REPLY		-21
#define LIBLAZY_ERROR_DBUS_ERROR_IS_SET		-22

/** @brief free a string
 *
 * @param string the string to free
 */
void liblazy_free_string(char *string);

/** @brief free a null terminated array of strings
 *
 * @param strlist the string list to free
 */
void liblazy_free_strlist(char **strlist);

/** @brief send a method call to the system bus
 *
 * sends a method call to the system bus. The call blocks if a reply is
 * given. If the call shouldn't block, give NULL for the reply
 *
 * @param destination the destination to send to
 * @param path the object path to send to
 * @param interface the interface to send to
 * @param method the method to send
 * @param reply a DBusMessage to store the reply or NULL if the call
 *              shouldn't block
 * @param first_arg_type a DBUS_TYPE_* of the fist argument
 * @param ... variable argument list finished with DBUS_TYPE_INVALID
 *
 * @return 0 on success, LIBLAZY_ERROR_* on failure
 */
int liblazy_dbus_system_send_method_call(const char *destination, const char *path,
					 const char *interface, const char *method,
					 DBusMessage **reply,
					 int first_arg_type, ...);

/** @brief send a method call to the session bus
 *
 * sends a method call to the session bus. The call blocks if a reply is
 * given. If the call shouldn't block, give NULL for the reply
 *
 * @param destination the destination to send to
 * @param path the object path to send to
 * @param interface the interface to send to
 * @param method the method to send
 * @param reply a DBusMessage to store the reply or NULL if the call
 *              shouldn't block
 * @param first_arg_type a DBUS_TYPE_* of the fist argument
 * @param ... variable argument list finished with DBUS_TYPE_INVALID
 *
 * @return 0 on success, LIBLAZY_ERROR_* on failure
 */

int liblazy_dbus_session_send_method_call(const char *destination, const char *path,
					  const char *interface, const char *method,
					  DBusMessage **reply,
					  int first_arg_type, ...);

/** @brief send a signal over the system bus
 *
 * sends a signal over the system bus. The call blocks if a reply is
 * given. If the call shouldn't block, give NULL for the reply
 *
 * @param path the object path to send to
 * @param interface the interface to send to
 * @param name the name of the signal
 * @param first_arg_type a DBUS_TYPE_* of the fist argument
 * @param ... variable argument list finished with DBUS_TYPE_INVALID
 *
 * @return 0 on success, LIBLAZY_ERROR_* on failure
 */
int liblazy_dbus_system_send_signal(const char *path, const char *interface,
				    const char *name, int first_arg_type, ...);

/** @brief send a signal over the session bus
 *
 * sends a signal over the session bus. The call blocks if a reply is
 * given. If the call shouldn't block, give NULL for the reply
 *
 * @param path the object path to send to
 * @param interface the interface to send to
 * @param name the name of the signal
 * @param first_arg_type a DBUS_TYPE_* of the fist argument
 * @param ... variable argument list finished with DBUS_TYPE_INVALID
 *
 * @return 0 on success, LIBLAZY_ERROR_* on failure
 */
int liblazy_dbus_session_send_signal(const char *path, const char *interface,
				     const char *name, int first_arg_type, ...);

/** @brief get an argument from a DBusMessage
 *
 * @param message the DBusMessage to get the argument from
 * @param type the type of the argument
 * @param arg a location to store the argument
 * @param no a number specifying the n'th argument of the given type
 *
 * @return 0 on success, LIBLAZY_ERROR_* on failure
 */
int liblazy_dbus_message_get_basic_arg(DBusMessage *message, int type,
				       void *arg, int no);

/** @brief get a array argument from a DBusMessage as a string list
 *
 * @param message the message to get the argument from
 * @param strlist a pointer to a string array to store the result
 * @param strlist pointer to array of strings to store the result. Has to
 *		  be freed with @ref liblazy_free_strlist
 * @param no a number specifying the n'th string array in the reply
 *
 * @return 0 on success, LIBLAZY_ERROR_* on failure
 */
int liblazy_dbus_message_get_strlist_arg(DBusMessage *message,
					 char ***strlist, int no);

/** @brief get integer property from HAL
 *
 * fetches one interger value from HAL.
 *
 * @param udi the device to fetch the value from
 * @param property the property to fetch
 * @param value location to store the result
 *
 * @return 0 on success, LIBLAZY_ERROR_* on failure
 */
int liblazy_hal_get_property_int(char *udi, char *property, int *value);

/** @brief get boolean property from HAL
 *
 * fetches one boolean value from HAL.
 *
 * @param udi the device to fetch the value from
 * @param property the property to fetch
 * @param value location to store the result
 *
 * @return 0 on success, LIBLAZY_ERROR_* on failure
 */
int liblazy_hal_get_property_bool(char *udi, char *property, int *value);

/** @brief get string property from HAL
 *
 * fetches one string value from HAL.
 *
 * @param udi the device to fetch the value from
 * @param property the property to fetch
 * @param value location to store the result
 *
 * @return 0 on success, LIBLAZY_ERROR_* on failure
 */
int liblazy_hal_get_property_string(char *udi, char *property, char **value);

/** @brief get string list property from HAL
 *
 * fetches a string list value from HAL.
 *
 * @param udi the device to fetch the value from
 * @param property the property to fetch
 * @param strlist pointer to array of strings to store the result. Has to
 *		  be freed with @ref liblazy_free_strlist
 *
 * @return 0 on success, LIBLAZY_ERROR_* on failure
 */
int liblazy_hal_get_property_strlist(char *udi, char *property, char ***strlist);

/** @brief check if a device has a capability
 *
 * @param udi the device to query on
 * @param capability the capability to query for
 *
 * @return 1 if the device possesses the capability, 0 if not and
 *         LIBLAZY_ERROR_* on failure
 */
int liblazy_hal_query_capability(char *udi, char *capability);

/** @brief find devices with a given capability
 *
 * @param capability the capability the devices should have
 * @param strlist pointer to array of strings to store the result. Has to
 *		  be freed with @ref liblazy_free_strlist
 *
 * @return 0 on success, LIBLAZY_ERROR_* on failure
 */
int liblazy_hal_find_device_by_capability(char *capability, char ***strlist);

/** @brief find devices with given key and value
 *
 * @param key the key to match against
 * @param value the value to match against
 * @param strlist pointer to array of strings to store the result. Has to
 *		  be freed with @ref liblazy_free_strlist
 *
 * @return 0 on success, LIBLAZY_ERROR_* on failure
 */
int liblazy_hal_find_device_by_string_match(char *key, char *value, char ***strlist);

/** @brief check if a user possesses a privilege
 *
 * Functions asks the PolicyKit daemon if a user possesses a given
 * privilege on a optional given ressource
 *
 * @param user the username to check for
 * @param privilege the privilege to check for
 * @param ressource the ressource to check for or NULL
 *
 * @return 0 on success, LIBLAZY_ERROR_* on failure
 */
int liblazy_polkit_is_user_allowed_by_name(char *user, char *privilege,
					   char *ressource);

/** @brief check if a user possesses a privilege
 *
 * Functions asks the PolicyKit daemon if a user possesses a given
 * privilege on a optional given ressource
 *
 * @param uid the uid to check for
 * @param privilege the privilege to check for
 * @param ressource the ressource to check for or NULL
 *
 * @return 0 on success, LIBLAZY_ERROR_* on failure
 */
int liblazy_polkit_is_user_allowed_by_uid(int uid, char *privilege,
					  char *ressource);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif				/* LIBLAZY_H */

/** @} */// end of liblazy group
