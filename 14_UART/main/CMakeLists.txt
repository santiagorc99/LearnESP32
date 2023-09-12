idf_component_register(
    SRCS 
        "main.c"
        "echo_example/echo_example.c"
        "PC_example/PC_example.c"
        "queue_example/queue_example.c"

    INCLUDE_DIRS 
        "."
        "echo_example"
        "PC_example"
        "queue_example"
)