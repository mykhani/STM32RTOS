[![License](https://img.shields.io/badge/License-BSD%203--Clause-blue.svg)](https://opensource.org/licenses/BSD-3-Clause)

# STM32 RTOS from scratch

Real time operating system from scratch (well almost), using the Cortex-M4 based STM32 Nucleo board and developlment tools from STM32CubeIDE, for learning purposes. Although it uses STM32 microcontroller and STM32 development tools, the concepts can be applied to create RTOS for any Cortex-M4 based boards.

## Roadmap
- [x] Implement basic Priority based task scheduler.
- [ ] Add basic I/O drivers
- [ ] Implement advanced RTOS mechanisms like memory protection (MPU), semaphores, mutexes, priority inversion, message queues, task management.
- [ ] Add various middlewares e.g. FAT filesystem, LwIP etc.
- [ ] Decouple from STM32CubeIDE so that RTOS can be used as a stand-alone library that embedded applications can link against. 
- [ ] Add the build system (most probably Kconfig based) to configure the RTOS once it grows and offers different configuration options.
- [ ] Add support for different STM32 boards.

## Boards supported

**1. STM32F401RE Nucleo**

![STM32F401RE](https://www.st.com/bin/ecommerce/api/image.PF260000.en.feature-description-include-personalized-no-cpn-large.jpg)
