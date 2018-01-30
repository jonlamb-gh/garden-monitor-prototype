/**
 * @file gui_util.h
 * @brief TODO.
 *
 */

#ifndef GUI_UTIL_H
#define GUI_UTIL_H

/**
 * @brief Minimum of a and b.
 *
 */
#ifndef MIN
#define MIN(a,b) ((a)<(b)?(a):(b))
#endif

/**
 * @brief Maximum of a and b.
 *
 */
#ifndef MAX
#define MAX(a,b) ((a)>(b)?(a):(b))
#endif

/**
 * @brief Absolute value of x.
 *
 */
#ifndef ABS
#define ABS(x) ((x)>0?(x):-(x))
#endif

/**
 * @brief Square of x.
 *
 */
#ifndef SQ
#define SQ(x) ((x)*(x))
#endif

/**
 * @brief Constrain amount to the range low:high.
 *
 */
#ifndef CONSTRAIN
#define CONSTRAIN(amt, low, high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))
#endif

#endif  /* GUI_UTIL_H */
