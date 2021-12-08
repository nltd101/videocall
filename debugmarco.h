#ifndef DEBUGMARCO_H
#define DEBUGMARCO_H

#define mdebug(x)                  \
  do                               \
  {                                \
    if (DEBUG_MODE)                \
    {                              \
      std::cerr << x << std::endl; \
    }                              \
  } while (0)
#define mlog(x)                    \
  do                               \
  {                                \
    if (DEBUG_MODE)                \
    {                              \
      std::cout << x << std::endl; \
    }                              \
  } while (0)
#endif // DEBUGMARCO_H
