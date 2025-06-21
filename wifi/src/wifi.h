#ifndef WIFI_H
#define WIFI_H

#include <string.h>
#include <zephyr/kernel.h>
#include <zephyr/net/wifi_mgmt.h>
#include <zephyr/net/dhcpv4_server.h>

const char *wifi_ssid = "your_wifi_ssid";
const char *wifi_psk = "your_wifi_password";

void init_wifi();
int connect_wifi(const char *ssid, const char *psk);
void get_ip_add();
int disconnect_wifi();

#endif
