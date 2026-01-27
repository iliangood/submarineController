#include "attitudeIndicator.h"
#include <stdexcept>
#include <iostream>
#include <filesystem>  // C++17+
#include <signal.h>
#include <sys/wait.h>

namespace fs = std::filesystem;

AttitudeIndicator::AttitudeIndicator() {
    // Определяем путь к папке AttitudeIndicator рядом с текущим исполняемым файлом
    fs::path exe_dir;

#ifdef _WIN32
    char path[MAX_PATH];
    GetModuleFileNameA(NULL, path, MAX_PATH);
    exe_dir = fs::path(path).parent_path();
#else
    // Linux/macOS: /proc/self/exe → путь к исполняемому файлу
    char path[4096];
    ssize_t len = readlink("/proc/self/exe", path, sizeof(path) - 1);
    if (len != -1) {
        path[len] = '\0';
        exe_dir = fs::path(path).parent_path();
    } else {
        // Fallback: текущая рабочая директория
        exe_dir = fs::current_path();
    }
#endif

    fs::path attitude_dir = exe_dir / "AttitudeIndicator";

    // Выбор платформы
#ifdef _WIN32
    launcher_path = (attitude_dir / "windows-amd64" / "AttitudeIndicator.exe").string();
#else
    launcher_path = (attitude_dir / "linux-amd64" / "AttitudeIndicator").string();
#endif

    // Проверка существования
    if (!fs::exists(launcher_path)) {
        throw std::runtime_error("Launcher not found: " + launcher_path + "\n"
                                 "Убедитесь, что папка AttitudeIndicator лежит рядом с исполняемым файлом.");
    }

    std::cout << "Launching Processing from: " << launcher_path << std::endl;

    launch();
}

AttitudeIndicator::~AttitudeIndicator() {
#ifdef _WIN32
    if (hProcess) {
        TerminateProcess(hProcess, 0);
        CloseHandle(hProcess);
    }
    if (hStdinWrite) {
        CloseHandle(hStdinWrite);
    }
#else
    if (child_pid > 0) {
        kill(child_pid, SIGTERM);
        waitpid(child_pid, nullptr, 0);
    }
    if (pipe_fd != -1) {
        close(pipe_fd);
    }
#endif
}

void AttitudeIndicator::sendAngles(double pitch, double roll, double depth) {
    std::string cmd = "pitch:" + std::to_string(pitch) +
                      " roll:" + std::to_string(roll) +
                      " depth:" + std::to_string(depth) + "\n";

#ifdef _WIN32
    if (hStdinWrite) {
        DWORD written;
        WriteFile(hStdinWrite, cmd.c_str(), cmd.size(), &written, nullptr);
    }
#else
    if (pipe_fd != -1) {
        write(pipe_fd, cmd.c_str(), cmd.size());
    }
#endif
}

void AttitudeIndicator::launch() {
#ifdef _WIN32
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

    std::string cmd = "\"" + launcher_path + "\"";

    if (!CreateProcess(nullptr, const_cast<char*>(cmd.c_str()), nullptr, nullptr, TRUE, 0, nullptr, nullptr, &si, &pi)) {
        throw std::runtime_error("Failed to launch launcher: " + std::to_string(GetLastError()));
    }

    hProcess = pi.hProcess;
    CloseHandle(pi.hThread);
    CloseHandle(hStdinRead);
#else
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        throw std::runtime_error("Failed to create pipe");
    }

    child_pid = fork();
    if (child_pid == 0) {
        close(pipefd[1]);
        dup2(pipefd[0], STDIN_FILENO);
        close(pipefd[0]);

        execl(launcher_path.c_str(), launcher_path.c_str(), (char*)nullptr);
        perror("execl launcher failed");
        exit(1);
    } else if (child_pid > 0) {
        close(pipefd[0]);
        pipe_fd = pipefd[1];
    } else {
        throw std::runtime_error("Fork failed");
    }
#endif
}