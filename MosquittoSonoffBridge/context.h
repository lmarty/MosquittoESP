void console(char *format, ...);
#define LOG(...)
// #define LOG(...) Serial.printf( __VA_ARGS__ )
#define DEBUG_WEB 1
#define DEBUG_ESP_PORT Serial;
#define addLog(level,line) LOG("(%d)%s",level,line)
