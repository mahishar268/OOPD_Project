#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <iterator>
#include <sstream>
#include <filesystem>
#include <regex>
#include <chrono>
#include <thread>
#include <future>

namespace fs = std::filesystem;

class ShellBase {
protected:
    bool wildcardMatch(const std::string& str, const std::string& pattern) {
        // Implement your wildcard matching logic here
        // For simplicity, let's consider '*' as a wildcard character
        // This implementation is a basic one and may not cover all cases

        size_t strIndex = 0;
        size_t patternIndex = 0;
        size_t strLength = str.length();
        size_t patternLength = pattern.length();

        while (strIndex < strLength && patternIndex < patternLength) {
            if (pattern[patternIndex] == '*') {
                // '*' matches any sequence of characters
                ++patternIndex;

                // If '*' is the last character in the pattern, it matches the remaining characters in str
                if (patternIndex == patternLength) {
                    return true;
                }

                // Find the next character after '*'
                while (strIndex < strLength && str[strIndex] != pattern[patternIndex]) {
                    ++strIndex;
                }
            } else if (pattern[patternIndex] == '?' || str[strIndex] == pattern[patternIndex]) {
                // '?' matches any single character
                // Or, characters match in the pattern and the string
                ++strIndex;
                ++patternIndex;
            } else {
                // Mismatch, reset to the beginning of the pattern
                patternIndex = 0;
            }
        }

        // Check if we reached the end of both the string and the pattern
        return strIndex == strLength && patternIndex == patternLength;
    }

    std::vector<std::string> tokenize(const std::string& input) {
        std::istringstream iss(input);
        return {std::istream_iterator<std::string>{iss}, std::istream_iterator<std::string>{}};
    }

    virtual void executeCommand(const std::string& command) = 0;
};

class SingleThreadedShell : public ShellBase {
public:
    void run() {
        while (true) {
            std::cout << "SingleThreadedShell$ ";
            std::string command;
            std::getline(std::cin, command);

            if (command == "exit") {
                break;
            }

            executeCommand(command);
        }
    }

private:

   bool wildcardMatch(const std::string& str, const std::string& pattern) {
    // Implement your wildcard matching logic here
    // For simplicity, let's consider '*' as a wildcard character
    // This implementation is a basic one and may not cover all cases

    size_t strIndex = 0;
    size_t patternIndex = 0;
    size_t strLength = str.length();
    size_t patternLength = pattern.length();

    while (strIndex < strLength && patternIndex < patternLength) {
        if (pattern[patternIndex] == '*') {
            // '*' matches any sequence of characters
            ++patternIndex;

            // If '*' is the last character in the pattern, it matches the remaining characters in str
            if (patternIndex == patternLength) {
                return true;
            }

            // Find the next character after '*'
            while (strIndex < strLength && str[strIndex] != pattern[patternIndex]) {
                ++strIndex;
            }
        } else if (pattern[patternIndex] == '?' || str[strIndex] == pattern[patternIndex]) {
            // '?' matches any single character
            // Or, characters match in the pattern and the string
            ++strIndex;
            ++patternIndex;
        } else {
            // Mismatch, reset to the beginning of the pattern
            patternIndex = 0;
        }
    }

    // Check if we reached the end of both the string and the pattern
    return strIndex == strLength && patternIndex == patternLength;
}
    std::vector<std::string> tokenize(const std::string& input) {
        std::istringstream iss(input);
        return {std::istream_iterator<std::string>{iss}, std::istream_iterator<std::string>{}};
    }

      void executeCommand(const std::string& command) override {
        std::vector<std::string> tokens = tokenize(command);

        if (tokens.empty()) {
            return;
        }

        std::string cmd = tokens[0];

        if (cmd == "cd") {
            changeDirectory(tokens);
        } else if (cmd == "ls") {
            listDirectory(tokens);
        } else if (cmd == "mv") {
            moveFile(tokens);
        } else if (cmd == "rm") {
            removeFile(tokens);
        } else if (cmd == "cp") {
            copyFile(tokens);
        } else if (cmd == "exit") {
            printExitHelp();
        } else {
            std::cout << "Command not recognized: " << cmd << std::endl;
        }
    }

    void printExitHelp() {
        std::cout << "exit command: Exit the shell." << std::endl;
    }

    void changeDirectory(const std::vector<std::string>& tokens) {
        if (std::find(tokens.begin(), tokens.end(), "-help") != tokens.end()) {
            printChangeDirectoryHelp();
            return;
        }

        if (tokens.size() < 2) {
            std::cout << "Usage: cd <directory>" << std::endl;
        } else {
            fs::current_path(tokens[1]);
        }
    }

    void printChangeDirectoryHelp() {
        std::cout << "cd command: Change current directory." << std::endl;
        std::cout << "Usage: cd <directory>" << std::endl;
    }

void listDirectory(const std::vector<std::string>& tokens) {
    bool allFiles = std::find(tokens.begin(), tokens.end(), "-a") != tokens.end();
    bool recursive = std::find(tokens.begin(), tokens.end(), "-r") != tokens.end();

    if (std::find(tokens.begin(), tokens.end(), "-help") != tokens.end()) {
        printListDirectoryHelp();
        return;
    }

    fs::directory_iterator end_itr;
    for (fs::directory_iterator itr(fs::current_path()); itr != end_itr; ++itr) {
        std::string filename = itr->path().filename().string();

        // Skip hidden files unless -a option is used
        if (!allFiles && filename[0] == '.' && filename != "..") {
            continue;
        }

        // Skip directories unless -a option is used
        if (!allFiles && fs::is_directory(itr->path())) {
            continue;
        }

        std::cout << filename << std::endl;

        // Recursively list all files in directories if -r option is used
        if (recursive && fs::is_directory(itr->path())) {
            listDirectoryRecursive(itr->path());
        }
    }
}


void listDirectoryRecursive(const fs::path& currentPath) {
    fs::directory_iterator end_itr;
    for (fs::directory_iterator itr(currentPath); itr != end_itr; ++itr) {
        std::string filename = itr->path().filename().string();
        std::cout << filename << std::endl;

        if (fs::is_directory(itr->path())) {
            listDirectoryRecursive(itr->path());
        }
    }
}


    void printListDirectoryHelp() {
        std::cout << "ls command: List files and directories." << std::endl;
        std::cout << "Usage: ls [OPTIONS] [PATTERN]" << std::endl;
        std::cout << "OPTIONS:" << std::endl;
        std::cout << "  -r          List files recursively." << std::endl;
        std::cout << "  -pattern    Specify a pattern to filter files." << std::endl;
        std::cout << "  -help       Display this help message." << std::endl;
    }

    void moveFile(const std::vector<std::string>& tokens) {
        if (std::find(tokens.begin(), tokens.end(), "-help") != tokens.end()) {
            printMoveFileHelp();
            return;
        }

        if (tokens.size() != 3) {
            std::cout << "Usage: mv <source> <destination>" << std::endl;
        } else {
            fs::rename(tokens[1], tokens[2]);
        }
    }

    void printMoveFileHelp() {
        std::cout << "mv command: Move files or directories." << std::endl;
        std::cout << "Usage: mv <source> <destination>" << std::endl;
    }

    void removeFile(const std::vector<std::string>& tokens) {
        if (std::find(tokens.begin(), tokens.end(), "-help") != tokens.end()) {
            printRemoveFileHelp();
            return;
        }

        bool force = std::find(tokens.begin(), tokens.end(), "-f") != tokens.end();

        for (size_t i = 1; i < tokens.size(); ++i) {
            if (force) {
                fs::remove_all(tokens[i]);
            } else {
                fs::remove(tokens[i]);
            }
        }
    }

    void printRemoveFileHelp() {
        std::cout << "rm command: Remove files or directories." << std::endl;
        std::cout << "Usage: rm [OPTIONS] <file/directory>" << std::endl;
        std::cout << "OPTIONS:" << std::endl;
        std::cout << "  -f          Force removal (remove without confirmation)." << std::endl;
        std::cout << "  -help       Display this help message." << std::endl;
    }

    void copyFile(const std::vector<std::string>& tokens) {
        if (std::find(tokens.begin(), tokens.end(), "-help") != tokens.end()) {
            printCopyFileHelp();
            return;
        }

        if (tokens.size() != 3) {
            std::cout << "Usage: cp <source> <destination>" << std::endl;
        } else {
            std::string source = tokens[1];
            std::string destination = tokens[2];

            // Check if the source file exists
            if (!fs::exists(source)) {
                std::cout << "Error: Source file does not exist." << std::endl;
                return;
            }

            // Check if the destination file already exists
            if (fs::exists(destination)) {
                std::cout << "Error: Destination file already exists." << std::endl;
                return;
            }

            try {
                fs::copy(source, destination);
                std::cout << "File copied successfully." << std::endl;
            } catch (const std::exception& e) {
                std::cerr << "Error: " << e.what() << std::endl;
            }
        }
    }

    void printCopyFileHelp() {
        std::cout << "cp command: Copy files or directories." << std::endl;
        std::cout << "Usage: cp <source> <destination>" << std::endl;
        std::cout << "  -help       Display this help message." << std::endl;
    }
};

class MultiThreadedShell : public ShellBase {
public:
    void run() {
        while (true) {
            std::cout << "MultiThreadedShell$ ";
            std::string command;
            std::getline(std::cin, command);

            if (command == "exit") {
                break;
            }
            auto future = std::async(std::launch::async, &MultiThreadedShell::executeCommand, this, command);
            future.get(); // Wait for the asynchronous task to finish
        }
    }

private:
    bool wildcardMatch(const std::string& str, const std::string& pattern) {
    // Implement your wildcard matching logic here
    // For simplicity, let's consider '*' as a wildcard character
    // This implementation is a basic one and may not cover all cases

    size_t strIndex = 0;
    size_t patternIndex = 0;
    size_t strLength = str.length();
    size_t patternLength = pattern.length();

    while (strIndex < strLength && patternIndex < patternLength) {
        if (pattern[patternIndex] == '*') {
            // '*' matches any sequence of characters
            ++patternIndex;

            // If '*' is the last character in the pattern, it matches the remaining characters in str
            if (patternIndex == patternLength) {
                return true;
            }

            // Find the next character after '*'
            while (strIndex < strLength && str[strIndex] != pattern[patternIndex]) {
                ++strIndex;
            }
        } else if (pattern[patternIndex] == '?' || str[strIndex] == pattern[patternIndex]) {
            // '?' matches any single character
            // Or, characters match in the pattern and the string
            ++strIndex;
            ++patternIndex;
        } else {
            // Mismatch, reset to the beginning of the pattern
            patternIndex = 0;
        }
    }

    // Check if we reached the end of both the string and the pattern
    return strIndex == strLength && patternIndex == patternLength;
}
    std::vector<std::string> tokenize(const std::string& input) {
        std::istringstream iss(input);
        return {std::istream_iterator<std::string>{iss}, std::istream_iterator<std::string>{}};
    }

    void executeCommand(const std::string& command) override {
        std::vector<std::string> tokens = tokenize(command);

        if (tokens.empty()) {
            return;
        }

        std::string cmd = tokens[0];

        if (cmd == "cd") {
            changeDirectory(tokens);
        } else if (cmd == "ls") {
            listDirectory(tokens);
        } else if (cmd == "mv") {
            moveFile(tokens);
        } else if (cmd == "rm") {
            removeFile(tokens);
        } else if (cmd == "cp") {
            copyFile(tokens);
        } else {
            std::cout << "Command not recognized: " << cmd << std::endl;
        }
    }
void changeDirectory(const std::vector<std::string>& tokens) {
        if (std::find(tokens.begin(), tokens.end(), "-help") != tokens.end()) {
            printChangeDirectoryHelp();
            return;
        }

        if (tokens.size() < 2) {
            std::cout << "Usage: cd <directory>" << std::endl;
        } else {
            fs::current_path(tokens[1]);
        }
    }

    void printChangeDirectoryHelp() {
        std::cout << "cd command: Change current directory." << std::endl;
        std::cout << "Usage: cd <directory>" << std::endl;
    }

void listDirectory(const std::vector<std::string>& tokens) {
    bool allFiles = std::find(tokens.begin(), tokens.end(), "-a") != tokens.end();
    bool recursive = std::find(tokens.begin(), tokens.end(), "-r") != tokens.end();

    if (std::find(tokens.begin(), tokens.end(), "-help") != tokens.end()) {
        printListDirectoryHelp();
        return;
    }

    fs::directory_iterator end_itr;
    for (fs::directory_iterator itr(fs::current_path()); itr != end_itr; ++itr) {
        std::string filename = itr->path().filename().string();

        // Skip hidden files unless -a option is used
        if (!allFiles && filename[0] == '.' && filename != "..") {
            continue;
        }

        // Skip directories unless -a option is used
        if (!allFiles && fs::is_directory(itr->path())) {
            continue;
        }

        std::cout << filename << std::endl;

        // Recursively list all files in directories if -r option is used
        if (recursive && fs::is_directory(itr->path())) {
            listDirectoryRecursive(itr->path());
        }
    }
}


void listDirectoryRecursive(const fs::path& currentPath) {
    fs::directory_iterator end_itr;
    for (fs::directory_iterator itr(currentPath); itr != end_itr; ++itr) {
        std::string filename = itr->path().filename().string();
        std::cout << filename << std::endl;

        if (fs::is_directory(itr->path())) {
            listDirectoryRecursive(itr->path());
        }
    }
}


    void printListDirectoryHelp() {
        std::cout << "ls command: List files and directories." << std::endl;
        std::cout << "Usage: ls [OPTIONS] [PATTERN]" << std::endl;
        std::cout << "OPTIONS:" << std::endl;
        std::cout << "  -r          List files recursively." << std::endl;
        std::cout << "  -pattern    Specify a pattern to filter files." << std::endl;
        std::cout << "  -help       Display this help message." << std::endl;
    }

    void moveFile(const std::vector<std::string>& tokens) {
        if (std::find(tokens.begin(), tokens.end(), "-help") != tokens.end()) {
            printMoveFileHelp();
            return;
        }

        if (tokens.size() != 3) {
            std::cout << "Usage: mv <source> <destination>" << std::endl;
        } else {
            fs::rename(tokens[1], tokens[2]);
        }
    }

    void printMoveFileHelp() {
        std::cout << "mv command: Move files or directories." << std::endl;
        std::cout << "Usage: mv <source> <destination>" << std::endl;
    }

    void removeFile(const std::vector<std::string>& tokens) {
        if (std::find(tokens.begin(), tokens.end(), "-help") != tokens.end()) {
            printRemoveFileHelp();
            return;
        }

        bool force = std::find(tokens.begin(), tokens.end(), "-f") != tokens.end();

        for (size_t i = 1; i < tokens.size(); ++i) {
            if (force) {
                fs::remove_all(tokens[i]);
            } else {
                fs::remove(tokens[i]);
            }
        }
    }

    void printRemoveFileHelp() {
        std::cout << "rm command: Remove files or directories." << std::endl;
        std::cout << "Usage: rm [OPTIONS] <file/directory>" << std::endl;
        std::cout << "OPTIONS:" << std::endl;
        std::cout << "  -f          Force removal (remove without confirmation)." << std::endl;
        std::cout << "  -help       Display this help message." << std::endl;
    }

    void copyFile(const std::vector<std::string>& tokens) {
        if (std::find(tokens.begin(), tokens.end(), "-help") != tokens.end()) {
            printCopyFileHelp();
            return;
        }

        if (tokens.size() != 3) {
            std::cout << "Usage: cp <source> <destination>" << std::endl;
        } else {
            std::string source = tokens[1];
            std::string destination = tokens[2];

            // Check if the source file exists
            if (!fs::exists(source)) {
                std::cout << "Error: Source file does not exist." << std::endl;
                return;
            }

            // Check if the destination file already exists
            if (fs::exists(destination)) {
                std::cout << "Error: Destination file already exists." << std::endl;
                return;
            }

            try {
                fs::copy(source, destination);
                std::cout << "File copied successfully." << std::endl;
            } catch (const std::exception& e) {
                std::cerr << "Error: " << e.what() << std::endl;
            }
        }
    }

    void printCopyFileHelp() {
        std::cout << "cp command: Copy files or directories." << std::endl;
        std::cout << "Usage: cp <source> <destination>" << std::endl;
        std::cout << "  -help       Display this help message." << std::endl;
    }
};

class PerformanceTester {
public:

    void runTests() {
        measurePerformance<SingleThreadedShell>("SingleThreadedShell", &SingleThreadedShell::run);
        measurePerformance<MultiThreadedShell>("MultiThreadedShell", &MultiThreadedShell::run);
    }

private:
    template <typename ShellType, typename FunctionType>
    void measurePerformance(const std::string& testName, FunctionType function) {
        auto start = std::chrono::high_resolution_clock::now();

        // Run the shell commands
        ShellType shell;
        (shell.*function)();

        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

        std::cout << testName << " took " << duration.count() << " milliseconds." << std::endl;
    }
};

int main() {
    PerformanceTester tester;
    tester.runTests();

    return 0;
}
