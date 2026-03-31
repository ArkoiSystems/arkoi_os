#ifndef KPANIC_H
#define KPANIC_H

#define KPANIC(format, ...) kpanic(format, __FILE__, __LINE__, __func__, __VA_ARGS__)

void kpanic(const char *format, const char* file, int line, const char* function, ...);

#endif //KPANIC_H
