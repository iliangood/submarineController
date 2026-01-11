// AttitudeIndicator.h (без изменений)
#ifndef PROCESSING_ATTITUDE_CONTROLLER_H
#define PROCESSING_ATTITUDE_CONTROLLER_H

#include <string>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/types.h>
#include <unistd.h>
#endif

class AttitudeIndicator {
public:
    AttitudeIndicator(const std::string& processing_path, const std::string& sketch_path);
    ~AttitudeIndicator() = default;
    void sendAngles(double pitch, double roll);

private:
    void launchProcessing(const std::string& processing_path, const std::string& sketch_path);

#ifdef _WIN32
    HANDLE hProcess = NULL;
    HANDLE hStdinWrite = NULL;
#else
    pid_t child_pid = -1;
    int pipe_fd = -1;
#endif
};

#endif