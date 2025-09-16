# Selene Enhancer

[![Magisk](https://img.shields.io/badge/Magisk-24%2B-green.svg)](https://github.com/topjohnwu/Magisk)
[![Platform](https://img.shields.io/badge/Platform-Android-lightgrey.svg)]()
[![Architecture](https://img.shields.io/badge/Arch-ARM64-blue.svg)]()
[![License](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

**Selene Enhancer** is a **Magisk module** powered by a **C++ binary**, designed to improve **performance, efficiency, and overall user experience** on Android devices.  
Unlike traditional shell-based modules, Selene Enhancer runs with **minimal overhead** and **faster execution**.

> ⚠️ **Note**: This module is specifically tuned and tested for the **Redmi 10 2022 (codename: selene)**.  
> While it may work on other ARM64 devices, best results are expected on **Selene**.  

---

## Features

### Performance & Responsiveness
- **Kernel Scheduler** → More responsive UI (`sched_latency_ns`, `sched_min_granularity_ns`)  
- **I/O Scheduler** → Faster data transfer (`nr_requests`, `read_ahead_kb`)  
- **CPU Governor** → All CPU cores set to `schedutil` (modern & efficient)  
- **Stune Boosting** → Extra priority for foreground and top-app processes  

### Memory & Battery Management
- **Adaptive Refresh Rate** → 60Hz (idle) ↔ 90Hz (active)  
- **Virtual Memory Tuning** → Adjusts `swappiness` & `vfs_cache_pressure`  
- **Low Memory Killer (LMK)** → Prevents overly aggressive background app killing  

### Network & System Tweaks
- **TCP Optimizations** → Fast Open, MTU Probing, and more  
- **Debloating Logs** → Disables unnecessary system logging/debugging  

---

## Requirements
- Android device with **Magisk v24.0+**  
- **ARM64 architecture**  
- Recommended device: **Redmi 10 2022 (selene)**  

---

## Installation
1. Download the `.zip` file from [Releases](../../releases).  
2. Open the **Magisk** app.  
3. Go to the **Modules** tab.  
4. Tap **Install from storage** → select the downloaded `.zip` file.  
5. Once installation is complete, tap **Reboot**.  

> ⚠️ This module is **systemless**, but use it at your own risk.  
> It is highly recommended to **backup your data** before installation.  

---

## License
This project is licensed under the **MIT License** – see the [LICENSE](LICENSE) file for details.  

---

## Acknowledgements
- Huge thanks to **[topjohnwu](https://github.com/topjohnwu)** for creating and maintaining **Magisk**, which makes this module possible.  
