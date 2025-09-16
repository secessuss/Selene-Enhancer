#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <unistd.h>
#include <cstdlib>
#include <cstdio>
#include <memory>
#include <array>
#include <dirent.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/input.h>

std::string exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        std::cerr << "popen() failed!" << std::endl;
        return "";
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    if (!result.empty() && result[result.length() - 1] == '\n') {
        result.erase(result.length() - 1);
    }
    return result;
}

void applyTweak(const std::string& path, const std::string& value) {
    if (access(path.c_str(), W_OK) == 0) {
        std::ofstream outfile(path);
        if (outfile.is_open()) {
            outfile << value;
            outfile.close();
        } else {
            std::cerr << "Failed to open for writing: " << path << std::endl;
        }
    }
}

void setProp(const std::string& key, const std::string& value) {
    std::string command = "setprop " + key + " " + value;
    system(command.c_str());
}

void waitForBoot() {
    while (exec("getprop sys.boot_completed") != "1") {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void applyOptimizerTweaks() {
    for (int i = 0; i < 8; ++i) {
        std::string gov_path = "/sys/devices/system/cpu/cpufreq/policy" + std::to_string(i) + "/scaling_governor";
        applyTweak(gov_path, "schedutil");
    }

    applyTweak("/proc/sys/vm/swappiness", "50");
    applyTweak("/proc/sys/vm/vfs_cache_pressure", "100");
    applyTweak("/proc/sys/vm/page-cluster", "0");

    setProp("lmk.minfree_levels", "0:55296,100:80640,200:106200,300:131760,900:197640,999:262144");

    if (access("/sys/block/mmcblk0/queue/nr_requests", F_OK) == 0) {
        applyTweak("/sys/block/mmcblk0/queue/nr_requests", "256");
        applyTweak("/sys/block/mmcblk0/queue/read_ahead_kb", "256");
    }

    applyTweak("/proc/sys/kernel/sched_latency_ns", "18000000");
    applyTweak("/proc/sys/kernel/sched_min_granularity_ns", "2250000");

    applyTweak("/dev/stune/foreground/schedtune.boost", "10");
    applyTweak("/dev/stune/top-app/schedtune.boost", "15");
    applyTweak("/dev/stune/top-app/schedtune.prefer_idle", "0");
    applyTweak("/dev/stune/background/schedtune.boost", "0");
    
    applyTweak("/proc/sys/net/ipv4/tcp_fastopen", "3");
    applyTweak("/proc/sys/net/ipv4/tcp_tw_reuse", "1");
    applyTweak("/proc/sys/net/ipv4/tcp_low_latency", "1");
    applyTweak("/proc/sys/net/ipv4/tcp_ecn", "1");
    applyTweak("/proc/sys/net/ipv4/tcp_mtu_probing", "1");

    setProp("persist.sys.lmk.reportkills", "false");
    setProp("persist.sys.mdlog_dumpback", "0");
    setProp("persist.vendor.dpm.loglevel", "0");
    setProp("persist.ims.disableADBLogs", "1");
    setProp("persist.ims.disableDebugLogs", "1");
    setProp("persist.ims.disableQXDMLogs", "1");
    setProp("persist.traced.enable", "0");
    setProp("persist.dbg.volte_avail_ovr", "0");
    setProp("persist.dbg.vt_avail_ovr", "0");
    setProp("persist.dbg.wfc_avail_ovr", "0");
    setProp("ro.debuggable", "0");
    setProp("dalvik.vm.mtk-stack-trace-file", "");
    setProp("dalvik.vm.minidebuginfo", "false");
    setProp("dalvik.vm.dex2oat-minidebuginfo", "false");
}

void adaptiveRefreshRateTask() {
    const std::string RATE_90HZ = "90.0";
    const std::string RATE_60HZ = "60.0";
    const int IDLE_TIMEOUT_SEC = 4;
    std::string touch_device_path = "";

    DIR *dir = opendir("/dev/input");
    if (dir == NULL) {
        return;
    }
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (std::string(entry->d_name).find("event") != std::string::npos) {
            std::string path = "/dev/input/" + std::string(entry->d_name);
            std::string cmd = "getevent -lp " + path;
            std::string output = exec(cmd.c_str());
            if (output.find("ABS_MT_POSITION_X") != std::string::npos || 
                output.find("ABS_MT_POSITION_Y") != std::string::npos) {
                touch_device_path = path;
                break;
            }
        }
    }
    closedir(dir);

    if (touch_device_path.empty()) {
        return;
    }

    int fd = open(touch_device_path.c_str(), O_RDONLY | O_NONBLOCK);
    if (fd < 0) {
        return;
    }

    auto set_refresh_rate = [&](const std::string& rate) {
        std::string current_rate = exec("settings get system min_refresh_rate");
        if (current_rate != rate) {
            std::string cmd = "settings put system min_refresh_rate " + rate;
            system(cmd.c_str());
        }
    };
    
    set_refresh_rate(RATE_60HZ);

    while (true) {
        fd_set read_fds;
        FD_ZERO(&read_fds);
        FD_SET(fd, &read_fds);

        struct timeval tv;
        tv.tv_sec = IDLE_TIMEOUT_SEC;
        tv.tv_usec = 0;

        int ret = select(fd + 1, &read_fds, nullptr, nullptr, &tv);

        if (ret > 0) {
            set_refresh_rate(RATE_90HZ);
            struct input_event ev[64];
            read(fd, ev, sizeof(struct input_event) * 64);
        } else if (ret == 0) {
            set_refresh_rate(RATE_60HZ);
        } else {
            break;
        }
    }
    close(fd);
}

int main() {
    waitForBoot();
    applyOptimizerTweaks();
    
    std::thread(adaptiveRefreshRateTask).detach();

    while (true) {
        std::this_thread::sleep_for(std::chrono::hours(1));
    }

    return 0;
}
