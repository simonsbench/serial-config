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

// #define MAX_SUBSCRIPTIONS 10

/* Function signature for a config edit callback notification
 *
 * Passes the base config tree pointer and int handle being referenced in the
 * subscribe call, and the config tree pointer changed in the edit.
 *
 * (The base tree pointer and the changed tree pointer will differ when the
 * recursive flag is set, and the base pointer is a collection in the path from
 * the config root to the changed field.)
 */
//typedef void (*update_event)(void *base_treeref, int handle, void *changed_treeref);


/* Subscribe to updates about config edits, at or below the tree reference.
 *
 * Returns an integer identifying the subscription, or -1 if MAX_SUBSCRIPTIONS
 * has been exceeded.
 *
 * Update callbacks are processed at the end of an update transaction, after
 * the config has been edited and committed.
 *
 * Set the recursive flag for edits made to collections or fields in the
 * subtree below the tree reference. Note that turning on recursive for any
 * subscription will prompt a config tree search for every edit; to check if
 * any edit is contained in a subtree subscription, the edit reference is
 * located in a full config tree iteration, then its reference path searched
 * for subscription updates. (Only one tree iteration is completed for each
 * commit.) Update performance for collection edits (lists, options, etc.) may
 * be higher performing than field edits.*/
//int subscribe(void *base_treeref, int recursive, update_event cb);

//void unsubscribe(int subscription);

