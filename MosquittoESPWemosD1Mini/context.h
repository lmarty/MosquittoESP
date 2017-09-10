void console(char *format, ...);
// #define LOG(...)
#define LOG(...) Serial.printf( __VA_ARGS__ )
#define DEBUG_WEB 1
#define DEBUG_ESP_PORT Serial;
#define LOG_WEB(...)  Serial.printf( __VA_ARGS__ )
// #define LOG_SERIAL Serial.printf
#define LOG_SERIAL(...) Serial.printf( __VA_ARGS__ )
#define LOG_ENV(...) Serial.printf( __VA_ARGS__ )
