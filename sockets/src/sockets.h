#ifndef WIFI_H
#define WIFI_H

#include <string.h>
#include <zephyr/kernel.h>
#include <zephyr/net/wifi_mgmt.h>
#include <zephyr/net/dhcpv4_server.h>
#include <zephyr/net/socket.h>

#define HTTP_HOST "httpbin.org"
#define HTTP_URL "/get"

char response [512];

const char *wifi_ssid = "MONIKA";
const char *wifi_psk = "shubika123";

void init_wifi();
int connect_wifi(const char *ssid, const char *psk);
void get_ip_add();
int disconnect_wifi();

#endif
