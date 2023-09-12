idf_component_register(
    SRCS 
        "main.c"
        "toggle_led.c"
        "push_button.c"
    
    INCLUDE_DIRS 
        ""
)

# Create a read only FAT
fatfs_create_rawflash_image(storage ../website/dist FLASH_IN_PROJECT)
