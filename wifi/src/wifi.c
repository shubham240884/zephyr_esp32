#include "wifi.h"

K_SEM_DEFINE(sem_wifi, 0, 1);
K_SEM_DEFINE(sem_ipv4, 0, 1);

static struct net_mgmt_event_callback wifi_cb;
static struct net_mgmt_event_callback ipv4_cb;

#define NET_EVENT_WIFI_MASK     NET_EVENT_WIFI_CONNECT_RESULT | NET_EVENT_WIFI_DISCONNECT_RESULT

static void wifi_event_handle(struct net_mgmt_event_callback *cb, uint32_t mgmt_event, struct net_if *iface) {
    const struct wifi_status *status = (const struct wifi_status *)cb->info;
    if (mgmt_event == NET_EVENT_WIFI_CONNECT_RESULT) {
        if (status->status) {
            printk("error connecting to wifi %d\n", status->status);
        } else {
            printk("connected \n");
            k_sem_give(&sem_wifi);
        }
    }
    if (mgmt_event == NET_EVENT_WIFI_DISCONNECT_RESULT) {
        if (status->status) {
            printk("disconnect failed with error %d", status->status);
        } else {
            printk("disconnected \n");
            k_sem_take(&sem_wifi, K_NO_WAIT);
        }
    }
}

static void ipv4_callback(struct net_mgmt_event_callback *cb, uint32_t mgmt_event, struct net_if *iface) {
    if (mgmt_event == NET_EVENT_IPV4_ADDR_ADD) {
        k_sem_give(&sem_ipv4);
    }
}

void init_wifi() {
    net_mgmt_init_event_callback(&wifi_cb, wifi_event_handle, NET_EVENT_WIFI_MASK);
    net_mgmt_add_event_callback(&wifi_cb);
    net_mgmt_init_event_callback(&ipv4_cb, ipv4_callback, NET_EVENT_IPV4_ADDR_ADD);
    net_mgmt_add_event_callback(&ipv4_cb);
}

int connect_wifi(const char *ssid, const char *psk) {
    int ret;

    struct net_if *iface;
    struct wifi_connect_req_params params;

    iface = net_if_get_default();

    params.ssid = ssid;
    params.ssid_length = strlen(ssid);
    params.psk = psk;
    params.psk_length = strlen(psk);
    params.security = WIFI_SECURITY_TYPE_PSK;
    params.band = WIFI_FREQ_BAND_UNKNOWN;
    params.channel = WIFI_CHANNEL_ANY;
    params.mfp = WIFI_MFP_OPTIONAL;

    ret = net_mgmt(NET_REQUEST_WIFI_CONNECT, iface, &params, sizeof(params));

    k_sem_take(&sem_wifi, K_FOREVER);
    return ret;
}

void get_ip_add() {
    struct wifi_iface_status status;
    struct net_if *iface;
    char ip_addr[NET_IPV4_ADDR_LEN];
    char gw_addr[NET_IPV4_ADDR_LEN];

    iface = net_if_get_default();

    k_sem_take(&sem_ipv4, K_FOREVER);

    if (net_mgmt(NET_REQUEST_WIFI_IFACE_STATUS, iface, &status, sizeof(struct wifi_iface_status))) {
        printk("failed to get wifi status\n");
    }

    memset(ip_addr, 0, NET_IPV4_ADDR_LEN);
    if (net_addr_ntop(AF_INET, &iface->config.ip.ipv4->unicast[0].ipv4.address.in_addr, ip_addr, sizeof(ip_addr)) == NULL) {
        printk("could not convert IP address to string\n");
    }
    memset(gw_addr, 0, NET_IPV4_ADDR_LEN);
    if (net_addr_ntop(AF_INET, &iface->config.ip.ipv4->gw, gw_addr, sizeof(gw_addr)) == NULL) {
        printk("could not convert gateway address to string\n");
    }

    printk("WiFi status :\r\n");
    if (status.state >= WIFI_STATE_ASSOCIATED) {
        printk("SSID : %-32s\r\n", status.ssid);
        printk("IP : %s\r\n", ip_addr);
        printk("Gateway : %s\r\n", gw_addr);
    }
}

int disconnect_wifi() {
    int ret;
    struct net_if *iface = net_if_get_default();
    ret = net_mgmt(NET_REQUEST_WIFI_DISCONNECT, iface, NULL, 0);
    return ret;
}

int main() {
    k_sleep(K_SECONDS(5));
    printk("init wifi\n");
    init_wifi();
    connect_wifi(wifi_ssid, wifi_psk);
    get_ip_add();
    disconnect_wifi();
    return 0;
}
