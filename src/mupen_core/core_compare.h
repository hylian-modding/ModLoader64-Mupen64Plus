#if !defined(COMPARE_CORE_H)
#define COMPARE_CORE_H

typedef enum {
  CORE_COMPARE_DISABLE = 0,
  CORE_COMPARE_SEND = 1,
  CORE_COMPARE_RECV = 2
} CoreCompareMode;

void compare_core_init(int mode);

#endif /* COMPARE_CORE_H */

