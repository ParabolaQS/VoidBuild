#include <iostream>
#include <cstdlib>
#include <string>
#include <unistd.h>

// ANSI Terminal Color Codes
const std::string RESET   = "\033[0m";
const std::string GREEN   = "\033[1;32m";
const std::string BLUE    = "\033[1;34m";
const std::string YELLOW  = "\033[1;33m";
const std::string RED     = "\033[1;31m";

// Helper function to execute system commands cleanly
void run_stage(const std::string& command, const std::string& description) {
    std::cout << BLUE << "==> " << description << RESET << "\n";
    
    int status = std::system(command.c_str());
    
    if (status != 0) {
        std::cerr << RED << "[-] Error executing: " << command << " (Exit Code: " << status << ")" << RESET << "\n";
        std::exit(status);
    }
    std::cout << GREEN << "[+] Stage completed successfully." << RESET << "\n\n";
}

int main() {
    // 1. Enforce root privileges natively via POSIX uid check
    if (getuid() != 0) {
        std::cerr << RED << "[-] Root privileges required. Please run with 'sudo'." << RESET << "\n";
        return 1;
    }

    // Clean, direct header text without the decorative lines
    std::cout << GREEN << "Void Linux Automated System Sync Tool" << RESET << "\n\n";

    // 2. Sync repositories and pull down package updates (and new kernels)
    run_stage("xbps-install -Syu", "Syncing repositories and upgrading all packages...");

    // 3. Clear out old package cache to free up disk space
    run_stage("xbps-remove -O", "Cleaning up obsolete cached package files...");

    // 4. Void specific: Purge old unlinked kernels to keep /boot immaculate
    run_stage("vkpurge rm all", "Safely purging old, unused Linux kernels...");

    std::cout << GREEN << "[+] System optimization complete!" << RESET << "\n";
    std::cout << YELLOW << "[!] If a new kernel package was installed, reboot to apply changes." << RESET << "\n";

    return 0;
}
