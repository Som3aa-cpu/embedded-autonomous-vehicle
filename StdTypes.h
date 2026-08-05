#ifndef __INTTYPES_H_
#define __INTTYPES_H_

typedef unsigned char         uint8_t;
typedef unsigned short int    uint16_t;
typedef unsigned long int     uint32_t;


typedef signed char           sint8_t;
typedef signed short int      sint16_t;
typedef signed long int       sint32_t;


typedef float                 float32_t;
typedef double                float64_t;
typedef long double           float128_t;
typedef void(*PF)(void);

#define StdTypes_OK        1
#define StdTypes_NOK       0

#define NULL    0

#endif /* STDINT_H_ */
