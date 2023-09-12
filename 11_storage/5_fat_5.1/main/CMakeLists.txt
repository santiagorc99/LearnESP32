idf_component_register(
    SRCS 
        "main.c"
    INCLUDE_DIRS 
        "."
)

# This is for FAT CRUD (Wear leveling)
# fatfs_create_spiflash_image(storage ../flash-loaded FLASH_IN_PROJECT)

# This is for FAT (Read only)
fatfs_create_rawflash_image(storage ../flash-loaded FLASH_IN_PROJECT)
