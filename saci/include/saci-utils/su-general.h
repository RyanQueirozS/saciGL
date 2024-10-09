#ifndef __SACI_UTILS_SU_GENERAL_H__
#define __SACI_UTILS_SU_GENERAL_H__

#ifdef __cplusplus
#define SACI_SCAST_TO(type) static_cast<type>
#else
#define SACI_SCAST_TO(type) (type)
#endif

#define SACI_ARRLEN(array) (sizeof(array) / sizeof(array[0]))

#endif
