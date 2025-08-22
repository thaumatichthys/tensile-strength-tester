#pragma once
#include <pico/stdlib.h>


bool AddTask(void (*function)(), uint32_t delayMicroseconds);
void UpdateTasks();
void SetErrorHandler( void (*errHandler)());