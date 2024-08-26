/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdint.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "tinyusb.h"
#include "tusb_cdc_acm.h"
#include "sdkconfig.h"
#define PRINTER_INTERFACE_CLASS    0x07  // Printer class code
#define PRINTER_INTERFACE_SUBCLASS 0x01  // Printer subclass code
#define PRINTER_INTERFACE_PROTOCOL 0x02  // Bidirectional interface protocol

// Device Descriptor
tusb_desc_device_t const desc_device = {
    .bLength            = sizeof(tusb_desc_device_t),
    .bDescriptorType    = TUSB_DESC_DEVICE,
    .bcdUSB             = 0x0200,
    .bDeviceClass       = 0x00,  // Defined at interface level
    .bDeviceSubClass    = 0x00,  // Defined at interface level
    .bDeviceProtocol    = 0x00,  // Defined at interface level
    .bMaxPacketSize0    = CFG_TUD_ENDPOINT0_SIZE,
    .idVendor           = 0xCafe,
    .idProduct          = 0x4000,
    .bcdDevice          = 0x0100,
    .iManufacturer      = 0x01,
    .iProduct           = 0x02,
    .iSerialNumber      = 0x03,
    .bNumConfigurations = 0x01
};

// Configuration Descriptor
uint8_t const desc_configuration[] = {
    // Configuration Descriptor
    0x09,                   // bLength
    TUSB_DESC_CONFIGURATION,// bDescriptorType
    0x20, 0x00,             // wTotalLength
    0x01,                   // bNumInterfaces
    0x01,                   // bConfigurationValue
    0x00,                   // iConfiguration
    0x80,                   // bmAttributes
    0x32,                   // bMaxPower

    // Interface Descriptor
    0x09,                   // bLength
    TUSB_DESC_INTERFACE,    // bDescriptorType
    0x00,                   // bInterfaceNumber
    0x00,                   // bAlternateSetting
    0x02,                   // bNumEndpoints
    PRINTER_INTERFACE_CLASS,// bInterfaceClass (Printer)
    PRINTER_INTERFACE_SUBCLASS, // bInterfaceSubClass (Printer)
    PRINTER_INTERFACE_PROTOCOL, // bInterfaceProtocol
    0x00,                   // iInterface

    // Endpoint OUT Descriptor (Bulk)
    0x07,                   // bLength
    TUSB_DESC_ENDPOINT,     // bDescriptorType
    0x01,                   // bEndpointAddress (OUT)
    TUSB_XFER_BULK,         // bmAttributes
    0x40, 0x00,             // wMaxPacketSize
    0x00,                   // bInterval

    // Endpoint IN Descriptor (Bulk)
    0x07,                   // bLength
    TUSB_DESC_ENDPOINT,     // bDescriptorType
    0x81,                   // bEndpointAddress (IN)
    TUSB_XFER_BULK,         // bmAttributes
    0x40, 0x00,             // wMaxPacketSize
    0x00,                   // bInterval
};

// String Descriptors
char const* string_desc[] = {
    (const char[]) { 0x09, 0x04 }, // 0: English (0x0409)
    "TinyUSB",                     // 1: Manufacturer
    "TinyUSB Printer",             // 2: Product
    "123456",                      // 3: Serials
};

// // Function to get the device descriptor
// uint8_t const * tud_descriptor_device_cb(void) {
//     return (uint8_t const*) &desc_device;
// }

// Function to get the configuration descriptor
// uint8_t const* tud_descriptor_configuration_cb(uint8_t index) {
//     (void) index;
//     return desc_configuration;
// }

// // Function to get string descriptor
// uint16_t const* tud_descriptor_string_cb(uint8_t index, uint16_t langid) {
//     return (uint16_t const*) string_desc[index];
// }

static const char *TAG = "example";

// Callback for handling class-specific requests
bool tud_printer_control_xfer_cb(uint8_t rhport, uint8_t stage, tusb_control_request_t const * request)
{
    // Handle class-specific control requests here if necessary
    return true;
}

// Callback for receiving data
uint32_t tud_printer_write_cb(void const * p_buffer, int bufsize)
{
    ESP_LOGI(TAG, "Received print data of size %u", bufsize);
    ESP_LOG_BUFFER_HEXDUMP(TAG, p_buffer, bufsize, ESP_LOG_INFO);
    return bufsize;
}

// Callback for handling printer interface
void tud_printer_init_cb(void)
{
    ESP_LOGI(TAG, "Printer interface initialized");
}

void app_main(void)
{
    ESP_LOGI(TAG, "USB initialization");

    const tinyusb_config_t tusb_cfg = {
        .device_descriptor = &desc_device,
        .string_descriptor = NULL,
        .external_phy = false,
        .configuration_descriptor = desc_configuration,
    };

    ESP_ERROR_CHECK(tinyusb_driver_install(&tusb_cfg));

    ESP_LOGI(TAG, "USB Printer initialization DONE");
}
