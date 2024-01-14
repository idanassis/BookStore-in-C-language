// Idan Assis - 208543496
// Maor Kayra - 316083492
// Dana Kon - 207703703

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <time.h>
#include "LogFunction.h"

#define LOG_FILENAME "log.log"

//writing to the log file
void logAction(const char* action) {
    // Get current time
    time_t currentTime = time(NULL);
    struct tm* timeInfo = localtime(&currentTime);
    char timeString[20];
    strftime(timeString, 20, "%Y-%m-%d %H:%M:%S", timeInfo);

    // Open log file in append mode
    FILE* logFile = fopen(LOG_FILENAME, "a");

    // Write log entry to file
    fprintf(logFile, "[%s] %s\n", timeString, action);

    // Close file
    fclose(logFile);
}