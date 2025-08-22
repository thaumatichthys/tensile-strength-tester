#include "TaskScheduler.h"


/*
    Similar to setTimeOut in JavaScript
*/

#define QUEUE_SIZE 128

typedef struct {
    uint32_t timeout;
    uint64_t timeWhenSet;
    void (*function)();
} QueuedFunction;

QueuedFunction queue[QUEUE_SIZE];
uint8_t queueLength = 0;

void (*errorHandler)();
bool errorHandlerSet = false;

bool AddTask(void (*function)(), uint32_t delayMicroseconds) {
    if (queueLength >= QUEUE_SIZE) {
        if (errorHandlerSet)
            errorHandler();
        return false;        
    }
    queue[queueLength].timeout = delayMicroseconds;
    queue[queueLength].function = function;
    queue[queueLength].timeWhenSet = to_us_since_boot(get_absolute_time());
    queueLength++;
    return true;
}

void UpdateTasks() {
    uint64_t time = to_us_since_boot(get_absolute_time());
    for (int i = 0; i < queueLength; i++) {
        if (time > (queue[i].timeout + queue[i].timeWhenSet)) {
            queue[i].function();
            queueLength--;
            // move the other array elements down to fill the gap
            for (int u = i; u < queueLength; u++)
                queue[u] = queue[u + 1];
        }
    }
}

void SetErrorHandler(void (*errHandler)()) {
    errorHandler = errHandler;
    errorHandlerSet = true;
}