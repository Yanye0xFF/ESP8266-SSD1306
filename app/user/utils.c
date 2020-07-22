/*
 * utils.c
 *
 *  Created on: Mar 31, 2020
 *      Author: Yanye
 */

#include "utils.h"


uint32_t iabs(int32_t value) {
	return (value < 0) ? (-value) : value;
}
