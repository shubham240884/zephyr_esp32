/*
 * ESP32 Bluetooth Enable Example for Zephyr RTOS
 * This program initializes and enables Bluetooth functionality
 */

#include <zephyr/kernel.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>

/* Device name for advertising */
#define DEVICE_NAME "ESP32_BT_Device"
#define DEVICE_NAME_LEN (sizeof(DEVICE_NAME) - 1)

/* Advertising data */
static const struct bt_data ad[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
    BT_DATA(BT_DATA_NAME_COMPLETE, DEVICE_NAME, DEVICE_NAME_LEN),
};

/* Connection callback functions */
static void connected(struct bt_conn *conn, uint8_t err)
{
    char addr[BT_ADDR_LE_STR_LEN];
    
    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
    
    if (err) {
        printk("Failed to connect to %s (err %u)\n", addr, err);
    } else {
        printk("Connected to %s\n", addr);
    }
}

static void disconnected(struct bt_conn *conn, uint8_t reason)
{
    char addr[BT_ADDR_LE_STR_LEN];
    
    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
    
    printk("Disconnected from %s (reason %u)\n", addr, reason);
}

BT_CONN_CB_DEFINE(conn_callbacks) = {
    .connected = connected,
    .disconnected = disconnected,
};

/* Function to start advertising */
static int start_advertising(void)
{
    int err;
    
    /* Use simple advertising start with predefined parameters */
    err = bt_le_adv_start(BT_LE_ADV_CONN, ad, ARRAY_SIZE(ad), NULL, 0);
    if (err) {
        printk("Advertising failed to start (err %d)\n", err);
        return err;
    }

    printk("Advertising started successfully\n");
    return 0;
}

/* Bluetooth ready callback */
static void bt_ready(int err)
{
    if (err) {
        printk("Bluetooth init failed (err %d)\n", err);
        return;
    }
    
    printk("Bluetooth initialized successfully\n");
    
    /* Start advertising */
    err = start_advertising();
    if (err) {
        printk("Failed to start advertising\n");
    }
}

int main(void)
{
    int err;
    
    printk("ESP32 Bluetooth Example Starting...\n");
    
    /* Initialize Bluetooth */
    err = bt_enable(bt_ready);
    if (err) {
        printk("Bluetooth init failed (err %d)\n", err);
        return -1;
    }
    
    printk("Bluetooth initialization in progress...\n");
    
    /* Main loop */
    while (1) {
        k_sleep(K_SECONDS(1));
        printk("Bluetooth running...\n");
    }
    
    return 0;
}
