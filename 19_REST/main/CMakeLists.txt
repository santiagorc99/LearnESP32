idf_component_register(
    SRCS 
        "main.c"
        "parse_weather.c"
                    
    INCLUDE_DIRS 
        "."

    EMBED_TXTFILES 
        "../cert/amazon.cer"
        "../cert/sendgridcert.cer"
)
