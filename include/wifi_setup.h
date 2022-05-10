#ifndef WIFI_SETUP_H
#define WIFI_SETUP_H

#ifdef __cplusplus
extern "C" {
#endif


/***** WIFI TEST CODE ***********/
#include <zephyr.h>
#include <assert.h>
#include <net/dns_resolve.h>
#include <net/sntp.h>
#include <net/net_conn_mgr.h>
#include <net/net_core.h>
#include <net/net_event.h>
#include <net/net_mgmt.h>
#include <wifi/eswifi/eswifi.h>
#include <net/wifi.h>
#include <net/wifi_mgmt.h>
#include <posix/time.h>
#include <random/rand32.h>
#include <config.h>

// static void set_system_time(const struct sntp_time *time) {
//     struct timespec ts = {
//         .tv_sec = time->seconds,
//         .tv_nsec = ((uint64_t) time->fraction * 1000000000) >> 32
//     };
//     if (clock_settime(CLOCK_REALTIME, &ts)) {
//         avs_log(zephyr_demo, WARNING, "Failed to set time");
//     }
// }

// void synchronize_clock(void) {
//     struct sntp_time time;
//     const uint32_t timeout_ms = 5000;
//     if (sntp_simple(CONFIG_ANJAY_CLIENT_NTP_SERVER, timeout_ms, &time)) {
//         avs_log(zephyr_demo, WARNING, "Failed to get current time");
//     } else {
//         set_system_time(&time);
//     }
// }


void initialize_network(void) {
    int status;
    printk("INFO: Initializing network connection...");
    struct net_if *iface = net_if_get_default();

    struct eswifi_dev *eswifi = eswifi_by_iface_idx(0);
    assert(eswifi);
    status = eswifi_at_cmd(eswifi, "I?\r ");  // Raw AT commands must be even length
    if (status < 0) {
        printk("ERROR: Failed to get info from eS-WiFI, received %d", status);
    }

    struct wifi_connect_req_params wifi_params = {
        .ssid = (uint8_t *) CONFIG_WIFI_SSID,
        .ssid_length = strlen(CONFIG_WIFI_SSID),
        .psk = (uint8_t *) CONFIG_WIFI_KEY,
        .psk_length = strlen(CONFIG_WIFI_KEY),
        .security = WIFI_SECURITY_TYPE_PSK
    };

    status = net_mgmt(NET_REQUEST_WIFI_CONNECT, iface, &wifi_params,
                      sizeof(struct wifi_connect_req_params));
    if (status) {
        printk("ERROR: Failed to configure Wi-Fi");
        exit(1);
    }

    net_mgmt_event_wait_on_iface(iface, NET_EVENT_IPV4_ADDR_ADD, NULL, NULL,
                                 NULL, K_FOREVER);
    printk("SUCCESS: Connected to network");
}


/****** END WIFI TEST CODE ********/




#ifdef __cplusplus
}
#endif

#endif // WIFI_SETUP_H
