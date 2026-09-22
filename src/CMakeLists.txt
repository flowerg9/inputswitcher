cmake_minimum_required(VERSION 3.10)
project(inputswitcher C)

add_executable(inputswitcher main.c)

add_custom_command(TARGET inputswitcher POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex -R .eeprom
            $<TARGET_FILE:inputswitcher> inputswitcher.hex
)
