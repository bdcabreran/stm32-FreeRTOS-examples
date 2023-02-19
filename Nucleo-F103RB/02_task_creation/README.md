# RTT SEGGER Library With STM32 Logger

## Difference Between ITM and RTT
ITM (Instrumentation Trace Macrocell) and RTT (Real-Time Transfer) are two debugging interfaces that are commonly used in ARM Cortex-M microcontrollers for real-time debugging and tracing.

The main difference between ITM and RTT is their intended use and the way they transfer data.

**ITM** is a **_hardware-based_** debugging interface that is built into the Cortex-M microcontroller. It provides a high-speed trace port that allows for real-time tracing of program execution, such as instruction execution, program counters, and data values. The ITM interface is typically used for advanced debugging and profiling purposes and can be used to capture and store large amounts of trace data for later analysis.

On the other hand, **RTT** is a **_software-based_** debugging interface that uses a circular buffer to transfer data between the target device and the host computer. RTT is intended for real-time communication and transfer of small amounts of data between the target device and the host computer. It is typically used for logging debug information, such as variable values, function call information, and error messages, in real-time.

In terms of implementation, ITM requires dedicated hardware pins for trace data and control signals, whereas RTT uses the standard SWD (Serial Wire Debug) interface for communication between the target device and the host computer. Additionally, ITM requires specialized debug probes that support trace capture, while RTT can be implemented using a standard SWD debug probe.

Overall, ITM and RTT are complementary debugging interfaces that can be used together to provide comprehensive debugging and tracing capabilities for Cortex-M microcontrollers.

  * `RTT/`
    * `SEGGER_RTT.c`               - Main module for RTT.
    * `SEGGER_RTT.h`               - Main header for RTT.
    * `SEGGER_RTT_ASM_ARMv7M.S`    - Assembly-optimized implementation of RTT functions for ARMv7M processors.
    * `SEGGER_RTT_Printf.c`        - Simple implementation of printf (`SEGGER_RTT_Printf()`) to write formatted strings via RTT.
  * `Syscalls/`
    * `SEGGER_RTT_Syscalls_*.c`    - Low-level syscalls to retarget `printf()` to RTT with different toolchains.
  * `Config/`
    * `SEGGER_RTT_Conf.h`          - RTT configuration file.