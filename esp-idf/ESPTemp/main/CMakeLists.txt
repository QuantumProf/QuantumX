idf_component_register(
    SRCS "main.c"
    INCLUDE_DIRS "."
    EMBED_TXTFILES "emqx_root_ca.pem"
    REQUIRES esp_wifi esp_event nvs_flash esp_netif mqtt esp_driver_tsens
)
