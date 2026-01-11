// AttitudeIndicator.cpp
#include "attitudeIndicator.h"
#include <iostream>
#include <stdexcept>
#include <string>

AttitudeIndicator::AttitudeIndicator(const std::string& processing_path, const std::string& sketch_path) {
    launchProcessing(processing_path, sketch_path);
}

// Destructor remains the same...

void AttitudeIndicator::sendAngles(double pitch, double roll) {
    std::string cmd = "pitch:" + std::to_string(pitch) + " roll:" + std::to_string(roll) + "\n";
#ifdef _WIN32
    if (hStdinWrite) {
        DWORD written;
        WriteFile(hStdinWrite, cmd.c_str(), cmd.size(), &written, NULL);
    }
#else
    if (pipe_fd != -1) {
        write(pipe_fd, cmd.c_str(), cmd.size());
    }
#endif
}

void AttitudeIndicator::launchProcessing(const std::string& processing_path, const std::string& sketch_path) {
#ifdef _WIN32
    // Windows: processing.exe cli --sketch="..." --present
    SECURITY_ATTRIBUTES saAttr{sizeof(SECURITY_ATTRIBUTES), nullptr, TRUE};

    HANDLE hStdinRead = nullptr;
    if (!CreatePipe(&hStdinRead, &hStdinWrite, &saAttr, 0)) {
        throw std::runtime_error("Failed to create pipe");
    }
    SetHandleInformation(hStdinWrite, HANDLE_FLAG_INHERIT, 0);

    STARTUPINFO si{};
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdInput = hStdinRead;
    si.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    si.hStdError = GetStdHandle(STD_ERROR_HANDLE);

    PROCESS_INFORMATION pi{};

    std::string cmd = "\"" + processing_path + "\" cli --sketch=\"" + sketch_path + "\" --present";

    if (!CreateProcess(nullptr, const_cast<char*>(cmd.c_str()), nullptr, nullptr, TRUE, 0, nullptr, nullptr, &si, &pi)) {
        throw std::runtime_error("Failed to launch Processing: " + std::to_string(GetLastError()));
    }

    hProcess = pi.hProcess;
    CloseHandle(pi.hThread);
    CloseHandle(hStdinRead);
#else
    // Linux/macOS: processing cli --sketch /path --present
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        throw std::runtime_error("Failed to create pipe");
    }

    child_pid = fork();
    if (child_pid == 0) {  // Child process
        close(pipefd[1]);  // Close write end
        dup2(pipefd[0], STDIN_FILENO);
        close(pipefd[0]);

        execlp(processing_path.c_str(), processing_path.c_str(),
               "cli",
               "--sketch", sketch_path.c_str(),
               "--present",
               (char*)nullptr);
        perror("execlp failed");
        exit(1);
    } else if (child_pid > 0) {
        close(pipefd[0]);  // Close read end
        pipe_fd = pipefd[1];
    } else {
        throw std::runtime_error("Fork failed");
    }
#endif
}