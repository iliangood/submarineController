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
    AttitudeIndicator();
    ~AttitudeIndicator();

    void sendAngles(double pitch, double roll, double depth);

private:
    void launch();

#ifdef _WIN32
    HANDLE hProcess = nullptr;
    HANDLE hStdinWrite = nullptr;
#else
    pid_t child_pid = -1;
    int pipe_fd = -1;
#endif

    std::string launcher_path;  // полный путь к исполняемому файлу
};

#endif