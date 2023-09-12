idf_component_register(
    SRCS 
        "minus.c"
    INCLUDE_DIRS 
        "."
# Requires is used to know what files it needs to include to compile
# In this case we are going to include the driver dependency.
    REQUIRES
        "driver"
)
