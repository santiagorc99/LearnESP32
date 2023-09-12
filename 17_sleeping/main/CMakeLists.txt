idf_component_register(
    SRCS 
        "main.c"
        "deep_sleep_example/deep_sleep_example.c"
        "hibernation_example/hibernation_example.c"
        "light_sleep_example/light_sleep_example.c"

    INCLUDE_DIRS 
        "."
        "deep_sleep_example"
        "hibernation_example"
        "light_sleep_example"
)
