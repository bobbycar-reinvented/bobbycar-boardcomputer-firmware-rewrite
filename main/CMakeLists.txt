file(
    GLOB_RECURSE files
    *.h
    *.cpp
)

set(dependencies
    freertos
    esp_system
    bobbycar-protocol
#    arduino-esp32
#    fmt
#    ArduinoJson
    espchrono
#    cpputils
#    espcpputils
#    FastLED-idf
    espconfiglib
#    espwifistack
#    sunset-idf
#    esphttpdutils
#    mdns
#    mqtt
#    esp_app_format
#    espasyncota
#    app_update
)

idf_component_register(
    SRCS
    ${files}
    INCLUDE_DIRS
    .
    REQUIRES
    ${dependencies}
)

execute_process(
        COMMAND git describe --tags --always --dirty --long
        WORKING_DIRECTORY ${COMPONENT_DIR}
        OUTPUT_VARIABLE GIT_VERSION
        OUTPUT_STRIP_TRAILING_WHITESPACE
)

message(STATUS "Git version: ${GIT_VERSION}")

target_compile_options(${COMPONENT_TARGET}
        PRIVATE
        -fstack-reuse=all
        -fstack-protector-all
        -fdiagnostics-color=always
        -Wno-unused-function
        -Wno-deprecated-declarations
        -Wno-missing-field-initializers
        -Wno-parentheses
        -DVERSION=\"${GIT_VERSION}\"
)