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

#include <stdlib.h>

void liblazy_free_string(char *string)
{
	if (string == NULL)
		return;
	free(string);
	string = NULL;
}

void liblazy_free_strlist(char **strlist)
{
	int i = 0;
	if (strlist == NULL)
		return;
	for (i = 0; strlist[i] != NULL; i++) {
		free(strlist[i]);
		strlist[i] = NULL;
	}
	free(strlist);
	strlist = NULL;
}

/** @mainpage liblazy
 *
 * \section sec_intro Introduction
 *
 * D-Bus methods provided for convenience
 * Liblazy is a simple and easy to use library that provides convenient
 * functions for sending messages over the D-Bus daemon, querying
 * information from HAL or asking PolicyKit for a privilege.
 */
