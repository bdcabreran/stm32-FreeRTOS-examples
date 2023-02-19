
# Logger Implementation using SWO ITM for Log Messages
This project provides an implementation of printf using SWO ITM for log messages. It includes a log library with different log levels, which can be used to log messages with different levels of severity. The log messages are printed in the console with colors and timestamps, making it easier to debug and troubleshoot.

## Requirements
1. A microcontroller that supports SWO ITM (e.g. Cortex-M microcontrollers)
2. A debugger that supports SWO ITM (e.g. J-Link)
3. An Integrated Development Environment (IDE) that supports your microcontroller and debugger
4. printf library

## Getting Started
1. Clone the repository or download the project files.
2. Include the log library in your project.
3. Initialize the log library with a log level and a function pointer that points to a _write_ function.

```c++

#include "log.h"

int main(void)
{
    log_init(LOG_LEVEL_DEBUG, &write);
    // Rest of your code
}
```

4. Use the log functions to print out messages. You can use the log functions without specifying a tag or with a custom tag. The tag will be printed along with the log message.

```c++
#include "log.h"

int main(void)
{
    log_init(LOG_LEVEL_DEBUG, &write_itm);
    log_message(LOG_LEVEL_DEBUG, "This is a debug message.\n");
    log_message_tag(LOG_LEVEL_WARNING, "custom-tag", "This is a warning message.\n");
    // Rest of your code
}
```
5. Compile and flash your project to your microcontroller.
6. Connect your debugger to your microcontroller and start a debugging session.
7. Open a console window in your IDE and enable SWO ITM. The SWO ITM settings may vary depending on your debugger and IDE.
8. Run your program and start logging messages. The log messages will appear in the console window with colors and timestamps.

## Configuration
You can customize the log library by changing the log level, the printf function pointer, and the color codes. The log level determines which messages will be printed. Messages with a log level lower than the current log level will not be printed. The printf function pointer determines where the log messages will be printed. The color codes determine the color of the log messages in the console window.

## Contributing
If you would like to contribute to this project, please open a pull request on GitHub.
