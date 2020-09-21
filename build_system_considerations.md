# Build system considerations

1. Cross-platform build support is not mandatory at the moment. (Only Linux build support should suffice)
2. Should follow Linux kernel's Kconfig as closely as possible. Here's the problem, the Linux
Kconfig has GPL license while this software has BSD license. I guess it is ok as long as
we don't link against Kconfig source and use it as a tool. Kconfig files are not part of the
tool I think (similar to how Makefiles can be written without worrying about Make tool's license).
What if we just keep Kconfig files in a separate directory with it's separate GPL license?
3. How to build third_party software their individual build systems? (like Cmake of lwIP)
Inspired by zephyr project (https://github.com/zephyrproject-rtos/zephyr), I've decided to
to go for CMake based build system owing to these advantages:
    1. Apache 2.0 license which is more permissable than GPL.
    2. Availability of good zephyr build system documentation. Since the primary goal of this
      development is a learning exercise, I feel I would be better able to develop a build
      system from scratch using that documentation as a reference.
    3. CMake build system is quite extensible and has good support for building the third
      party software with their different build systems. Integrating third party software,
      hence would be easy.
    4. I really like the Zephyr source tree structure. It resembles the structure of the Linux
      kernel source tree which is my preferred structure due to personal choice. 
