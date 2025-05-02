// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"
// #include "freertos/event_groups.h"
// #include "esp_log.h"
// #include "driver/temperature_sensor.h"
// #include "esp_sleep.h"
// #include "esp_timer.h"
// #include "esp_wifi.h"
// #include "esp_event.h"
// #include "nvs_flash.h"
// #include "esp_netif.h"
// #include "mqtt_client.h"
// #include "dtf_ota.h"

// extern const uint8_t emqx_root_ca_pem_start[] asm("_binary_emqx_root_ca_pem_start");
// extern const uint8_t emqx_root_ca_pem_end[] asm("_binary_emqx_root_ca_pem_end");

// static const char *TAG = "ESP32-C6 Temp Monitor";

// #define WAKE_DURATION_SEC   30
// #define SLEEP_DURATION_SEC  300

// // WiFi configuration - replace with your credentials
// #define WIFI_SSID "Hidden Network"
// #define WIFI_PASS "xxxxxxxxx"

// // MQTT configuration - replace with your broker details
// #define MQTT_BROKER_URI "mqtts://xxxxxxxxxxxxx.com:8883"
// #define MQTT_TOPIC "esp32c6/temperature"

// #define WIFI_CONNECTED_BIT BIT0
// static EventGroupHandle_t wifi_event_group;
// static esp_mqtt_client_handle_t mqtt_client = NULL;

// // --- WiFi Event Handler ---
// static void wifi_event_handler(void *arg, esp_event_base_t event_base,
//                                int32_t event_id, void *event_data) {
//     if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
//         esp_wifi_connect();
//     } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
//         ESP_LOGI(TAG, "Disconnected from WiFi. Reconnecting...");
//         esp_wifi_connect();
//     } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
//         ip_event_got_ip_t *event = (ip_event_got_ip_t *) event_data;
//         ESP_LOGI(TAG, "Got IP: " IPSTR, IP2STR(&event->ip_info.ip));
//         xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_BIT);
//     }
// }

// // --- WiFi Init ---
// static void wifi_init_sta(void) {
//     wifi_event_group = xEventGroupCreate();

//     ESP_ERROR_CHECK(esp_netif_init());
//     ESP_ERROR_CHECK(esp_event_loop_create_default());
//     esp_netif_create_default_wifi_sta();

//     wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
//     ESP_ERROR_CHECK(esp_wifi_init(&cfg));

//     esp_event_handler_instance_t instance_any_id;
//     esp_event_handler_instance_t instance_got_ip;
//     ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
//                                                         ESP_EVENT_ANY_ID,
//                                                         &wifi_event_handler,
//                                                         NULL,
//                                                         &instance_any_id));
//     ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
//                                                         IP_EVENT_STA_GOT_IP,
//                                                         &wifi_event_handler,
//                                                         NULL,
//                                                         &instance_got_ip));

//     wifi_config_t wifi_config = {
//         .sta = {
//             .ssid = WIFI_SSID,
//             .password = WIFI_PASS,
//             .threshold.authmode = WIFI_AUTH_WPA2_PSK,
//         },
//     };

//     ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
//     ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
//     ESP_ERROR_CHECK(esp_wifi_start());

//     ESP_LOGI(TAG, "Waiting for WiFi connection...");
//     EventBits_t bits = xEventGroupWaitBits(wifi_event_group,
//                                            WIFI_CONNECTED_BIT,
//                                            pdFALSE,
//                                            pdFALSE,
//                                            pdMS_TO_TICKS(15000));

//     if (bits & WIFI_CONNECTED_BIT) {
//         ESP_LOGI(TAG, "WiFi Connected!");
//     } else {
//         ESP_LOGE(TAG, "WiFi connection failed.");
//     }
// }

// // --- MQTT Event Handler ---
// static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
//     esp_mqtt_event_handle_t event = event_data;
//     switch (event->event_id) {
//         case MQTT_EVENT_CONNECTED:
//             ESP_LOGI(TAG, "MQTT Connected");
//             break;
//         case MQTT_EVENT_DISCONNECTED:
//             ESP_LOGI(TAG, "MQTT Disconnected");
//             break;
//         case MQTT_EVENT_ERROR:
//             ESP_LOGI(TAG, "MQTT Error");
//             break;
//         default:
//             break;
//     }
// }

// // --- MQTT Init ---
// static void mqtt_app_start(void) {
//     esp_mqtt_client_config_t mqtt_cfg = {
//         .broker.address.uri = MQTT_BROKER_URI,
//         .broker.verification.certificate = (const char *)emqx_root_ca_pem_start,
//         .credentials = {
//             .username = "xxxxxxxxxx", // Fill if needed
//             .authentication.password = "xxxxxxxxxxx", // Fill if needed
//         },
//     };


//     mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
//     esp_mqtt_client_register_event(mqtt_client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
//     esp_mqtt_client_start(mqtt_client);
// }

// // --- Publish ---
// void publish_temperature(float temperature) {
//     if (mqtt_client) {
//         char message[50];
//         snprintf(message, sizeof(message), "%.2f", temperature);
//         esp_mqtt_client_publish(mqtt_client, MQTT_TOPIC, message, 0, 1, 0);
//     }
// }

// // --- Main ---
// void app_main() {
//     // Init NVS
//     esp_err_t ret = nvs_flash_init();
//     if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
//         ESP_ERROR_CHECK(nvs_flash_erase());
//         ret = nvs_flash_init();
//     }
//     ESP_ERROR_CHECK(ret);

//     // Init WiFi
//     wifi_init_sta();

//     const dtf_ota_cfg_t cfg = {
//         .product_id = "xxxxxxxxxxxxx",   
//         .reboot_option = DTF_NO_REBOOT           
//     };

//     DTF_OtaResponse ota_ret = dtf_get_firmware_update(&cfg);

//     if (ota_ret == DTFOTA_NewFirmwareFlashed) {
//         ESP_LOGI(TAG, "OTA update successful");
//     } else {
//         ESP_LOGI(TAG, "No OTA update available or an error occurred (code %d)", ota_ret);
//     }

//     // Start MQTT
//     mqtt_app_start();

//     // Init temperature sensor
//     int64_t start_time = esp_timer_get_time();
//     temperature_sensor_handle_t temp_handle = NULL;
//     temperature_sensor_config_t temp_sensor = {
//         .range_min = -10,
//         .range_max = 80,
//     };
//     ESP_ERROR_CHECK(temperature_sensor_install(&temp_sensor, &temp_handle));
//     ESP_ERROR_CHECK(temperature_sensor_enable(temp_handle));

//     // Wait for a few seconds
//     vTaskDelay(5000 / portTICK_PERIOD_MS);

//     // Run for WAKE_DURATION_SEC
//     while ((esp_timer_get_time() - start_time) < (WAKE_DURATION_SEC * 1000000)) {
//         float temperature;
//         ESP_ERROR_CHECK(temperature_sensor_get_celsius(temp_handle, &temperature));
//         ESP_LOGI(TAG, "Temperature: %.2f °C", temperature);
//         publish_temperature(temperature);
//         vTaskDelay(2000 / portTICK_PERIOD_MS);
//     }

//     // Clean up
//     ESP_ERROR_CHECK(temperature_sensor_disable(temp_handle));
//     if (mqtt_client) {
//         esp_mqtt_client_disconnect(mqtt_client);
//         esp_mqtt_client_stop(mqtt_client);
//     }
//     esp_wifi_disconnect();
//     esp_wifi_stop();

//     ESP_LOGI(TAG, "Going to deep sleep for %d seconds...", SLEEP_DURATION_SEC);
//     esp_sleep_enable_timer_wakeup(SLEEP_DURATION_SEC * 1000000);
//     esp_deep_sleep_start();
// }













#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "esp_sleep.h"
#include "esp_timer.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "mqtt_client.h"
#include "dtf_ota.h"
#include "esp_random.h"
#include "esp_ota_ops.h"

extern const uint8_t emqx_root_ca_pem_start[] asm("_binary_emqx_root_ca_pem_start");
extern const uint8_t emqx_root_ca_pem_end[] asm("_binary_emqx_root_ca_pem_end");

static const char *TAG = "ESP32-C6 Temp Monitor";

#define WAKE_DURATION_SEC   30
#define SLEEP_DURATION_SEC  300

// WiFi configuration - replace with your credentials
#define WIFI_SSID "Hidden Network"
#define WIFI_PASS "xxxxxxxxxxx"

// MQTT configuration - replace with your broker details
#define MQTT_BROKER_URI "mqtts://xxxxxxxxxxxxxxx.com:8883"
#define MQTT_TOPIC "esp32c6/temperature"

#define WIFI_CONNECTED_BIT BIT0
static EventGroupHandle_t wifi_event_group;
static esp_mqtt_client_handle_t mqtt_client = NULL;

// --- WiFi Event Handler ---
static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data) {
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        ESP_LOGI(TAG, "Disconnected from WiFi. Reconnecting...");
        esp_wifi_connect();
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *) event_data;
        ESP_LOGI(TAG, "Got IP: " IPSTR, IP2STR(&event->ip_info.ip));
        xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

// --- WiFi Init ---
static void wifi_init_sta(void) {
    wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        &instance_got_ip));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
        },
    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "Waiting for WiFi connection...");
    EventBits_t bits = xEventGroupWaitBits(wifi_event_group,
                                           WIFI_CONNECTED_BIT,
                                           pdFALSE,
                                           pdFALSE,
                                           pdMS_TO_TICKS(15000));

    if (bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI(TAG, "WiFi Connected!");
    } else {
        ESP_LOGE(TAG, "WiFi connection failed.");
    }
}

// --- MQTT Event Handler ---
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    esp_mqtt_event_handle_t event = event_data;
    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT Connected");
            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT Disconnected");
            break;
        case MQTT_EVENT_ERROR:
            ESP_LOGI(TAG, "MQTT Error");
            break;
        default:
            break;
    }
}

// --- MQTT Init ---
static void mqtt_app_start(void) {
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = MQTT_BROKER_URI,
        .broker.verification.certificate = (const char *)emqx_root_ca_pem_start,
        .credentials = {
            .username = "xxxxxxxxx", // Fill if needed
            .authentication.password = "xxxxxxxxxxx", // Fill if needed
        },
    };

    mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(mqtt_client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(mqtt_client);
}

// --- Publish ---
void publish_temperature(float temperature) {
    if (mqtt_client) {
        char message[50];
        snprintf(message, sizeof(message), "%.2f", temperature);
        esp_mqtt_client_publish(mqtt_client, MQTT_TOPIC, message, 0, 1, 0);
    }
}

// --- Generate random temperature between 1-5°C to check if the cpu temperature is replaced by random generated random numbers ---
float generate_random_temperature() {
    // Generate random number between 0 and 4, then add 1 to get 1-5 range
    uint32_t random_val = esp_random() % 5;
    float temperature = (float)random_val + 1.0f;
    
    // Add some decimal variation (0.00-0.99)
    temperature += (float)(esp_random() % 100) / 100.0f;
    
    return temperature;
}

// --- Main ---
void app_main() {
    // Init NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Init WiFi
    wifi_init_sta();

    const dtf_ota_cfg_t cfg = {
        .product_id = "xxxxxxxxxxxxxxxx",   // <<< Replace with your Deploy the Fleet product ID
        .reboot_option = DTF_REBOOT_ON_SUCCESS          
    };

    DTF_OtaResponse ota_ret = dtf_get_firmware_update(&cfg);

    if (ota_ret == DTFOTA_NewFirmwareFlashed) {
        ESP_LOGI(TAG, "OTA update successful");
        esp_ota_mark_app_valid_cancel_rollback();
    } else {
        ESP_LOGI(TAG, "No OTA update available or an error occurred (code %d)", ota_ret);
    }

    // Start MQTT
    mqtt_app_start();

    int64_t start_time = esp_timer_get_time();

    // Wait for a few seconds
    vTaskDelay(5000 / portTICK_PERIOD_MS);

    // Run for WAKE_DURATION_SEC
    while ((esp_timer_get_time() - start_time) < (WAKE_DURATION_SEC * 1000000)) {
        float temperature = generate_random_temperature();
        ESP_LOGI(TAG, "Temperature: %.2f °C", temperature);
        publish_temperature(temperature);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }

    // Clean up
    if (mqtt_client) {
        esp_mqtt_client_disconnect(mqtt_client);
        esp_mqtt_client_stop(mqtt_client);
    }
    esp_wifi_disconnect();
    esp_wifi_stop();

    ESP_LOGI(TAG, "Going to deep sleep for %d seconds...", SLEEP_DURATION_SEC);
    esp_sleep_enable_timer_wakeup(SLEEP_DURATION_SEC * 1000000);
    esp_deep_sleep_start();
}