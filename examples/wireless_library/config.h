#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>


struct string {
    const uint16_t capacity;
    uint16_t length;
    char list[255];
};
struct wireless_settings {
    const struct string *network_ssid;
    const struct string *network_password;
};
struct polling_settings {
    uint16_t poll_interval_mins;
};
struct humidity sensor {
    const struct wireless_settings *wireless_settings;
    const struct polling_settings *sensor_settings;
};

void init_config();

struct humidity sensor * read_config();

/* Commit the current config to storage
 *
 * Use this commit function to persist external edits made to the config tree;
 * config updates handled internally are automatically committed.
 */
void commit_config();

uint8_t current_slot();

const uint8_t slots = 2;

void set_slot(uint8_t slot);

/* Commit the current config into the store */
void store_config();

/* Drop into checking for a serial update */
bool handle_serial_update();
