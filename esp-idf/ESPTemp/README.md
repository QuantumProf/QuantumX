Supported Targets:
- ESP32-C2
- ESP32-C3
- ESP32-C5
- ESP32-C6
- ESP32-H2
- ESP32-P4
- ESP32-S2
- ESP32-S3


# ESP32 MQTT Temperature Monitor

This project uses ESP32-C6 to read temperature data and publish it securely over MQTT to an EMQX broker. The data can be forwarded to Firebase or other cloud services via EMQX's data integration tools.

## Features

- Secure MQTT communication (MQTTS)
- Sensor reading and logging (e.g., internal temperature sensor)
- Integration with EMQX Cloud (TLS supported)
- Optionally forward data to Firebase for visualization

## Hardware Used

- ESP32 or ESP32-C6
- Onboard temperature sensor (or external if used)
- Wi-Fi network

## Project Structure

ESPTemp/
├── main/
│   ├── main.c
│   ├── emqx_root_ca.pem      
│   └── CMakeLists.txt
├── sdkconfig
├── CMakeLists.txt
├── README.md
└── build/                    <- Generated during build (add to .gitignore)


### Certificate Setup

To use secure MQTT (`mqtts://`), you need to provide the EMQX broker's **Root CA Certificate**.

#### Steps to Download and Save the Root CA Certificate:
1. Open Git Bash in your project directory.
2. Run the following command to download the EMQX Root CA certificate:

    ```bash
    openssl s_client -connect broker.emqx.io:8883 -showcerts </dev/null 2>/dev/null | \
    openssl x509 -outform PEM -out emqx_root_ca.pem
    ```

3. Open the `emqx_root_ca.pem` file in a text editor and copy the entire content.
4. Save the content as a `.pem` file inside the `main/` directory of your project.

#### File Reference in Code:

Once the certificate is saved, you will reference it in your code as follows:

```c
extern const uint8_t emqx_root_ca_pem_start[] asm("_binary_emqx_root_ca_pem_start");
extern const uint8_t emqx_root_ca_pem_end[] asm("_binary_emqx_root_ca_pem_end");
