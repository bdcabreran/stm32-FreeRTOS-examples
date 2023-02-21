# Queue FreeRTOs example

In FreeRTOS, a semaphore is a synchronization object used to manage access to shared resources between tasks. It is a special type of binary semaphore that can be used to signal the availability of a resource, and to block a task until that resource is available.

## Queue Passing byte by Value:

* In this example, a single uint8_t is set up to pass individual enumerations, (LED_CMDS), defining the state of one LED at a time or all of the LEDs (on/off)

