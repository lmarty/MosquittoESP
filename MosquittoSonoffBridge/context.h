void console(char *format, ...);
#undef DEBUG

#ifdef DEBUG
#define LOG(...) Serial.printf( __VA_ARGS__ )
#define DEBUG_WEB 1
#define DEBUG_ESP_PORT Serial;
#define LOG_WEB(...)  Serial.printf( __VA_ARGS__ )
#define LOG_SERIAL(...) Serial.printf( __VA_ARGS__ )
#define LOG_ENV(...) Serial.printf( __VA_ARGS__ )
#else
#define LOG_SERIAL(...)
#define LOG(...)
#define LOG_WEB(...)
#define LOG_ENV(...)
#define DEBUG_ESP_PORT
#endif


