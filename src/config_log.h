#ifndef CONFIG_LOG_H
#define CONFIG_LOG_H

#if DEBUG
#define LOG_LEVEL LOG_LEVEL_DBG
#else
#define LOG_LEVEL LOG_LEVEL_INF
#endif

#endif
