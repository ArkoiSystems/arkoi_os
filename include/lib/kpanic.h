#ifndef PANIC_H
#define PANIC_H

#define PANIC(format, ...) panic(format, __FILE__, __LINE__, __func__, __VA_ARGS__)

void panic(const char *format, const char* file, int line, const char* function, ...);

#endif //PANIC_H
