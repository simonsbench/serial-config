#include "config.h"
#include <stdlib.h>
#include <FlashAsEEPROM.h>

// Send a debug message over the wire
#define proto_debug 1
void proto_print(String str);

/* INSTANCE */

struct string id_0 = {255, 255, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};

struct string id_1 = {255, 255, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};

struct wireless_settings id_2 = {&id_0, &id_1};

struct polling_settings id_3 = {0};

struct humidity sensor id_4 = {&id_2, &id_3};

/* ACCESSOR AND TYPE LABELS */
enum ACCESSOR_LIST_GENERIC {
    ACCESSOR_LIST_GENERIC_capacity,
    ACCESSOR_LIST_GENERIC_length,
    ACCESSOR_LIST_GENERIC_type
};

// Member accessor for struct wireless_settings id_2
enum ACCESSOR_STRUCT_wireless_settings {
    ACCESSOR_STRUCT_wireless_settings_network_ssid,
    ACCESSOR_STRUCT_wireless_settings_network_password
};

// Member accessor for struct polling_settings id_3
enum ACCESSOR_STRUCT_polling_settings {
    ACCESSOR_STRUCT_polling_settings_poll_interval_mins
};

// Member accessor for struct humidity sensor id_4
enum ACCESSOR_STRUCT_humidity sensor {
    ACCESSOR_STRUCT_humidity sensor_wireless_settings,
    ACCESSOR_STRUCT_humidity sensor_sensor_settings
};

enum ident_type {
    IDENT_char,
    IDENT_uint16_t,
    IDENT__END_OF_PRIMITIVES,
    IDENT_LIST_char,
    IDENT__END_OF_LISTS,
    IDENT_STRUCT_wireless_settings,
    IDENT_STRUCT_polling_settings,
    IDENT_STRUCT_humidity sensor,
    IDENT__END_OF_STRUCTS
};

const int ident_sizes[] = {
    sizeof(char),
    sizeof(uint16_t),
    -1,
    sizeof(struct string),
    -1,
    sizeof(struct wireless_settings),
    sizeof(struct polling_settings),
    sizeof(struct humidity sensor),
    -1
};

const char *ident_labels[] = {
    "IDENT_char",
    "IDENT_uint16_t",
    "IDENT__END_OF_PRIMITIVES",
    "IDENT_LIST_char",
    "IDENT__END_OF_LISTS",
    "IDENT_STRUCT_wireless_settings",
    "IDENT_STRUCT_polling_settings",
    "IDENT_STRUCT_humidity sensor",
    "IDENT__END_OF_STRUCTS"
};

#define ident_is_primitive(ident) ((ident) >= 0 && (ident) < IDENT__END_OF_PRIMITIVES)
#define ident_is_list(ident)      ((ident) > IDENT__END_OF_PRIMITIVES && (ident) < IDENT__END_OF_LISTS)
#define ident_is_struct(ident)    ((ident) > IDENT__END_OF_LISTS && (ident) < IDENT__END_OF_STRUCTS)

#define ROOT_STRUCT_REF &id_4
#define ROOT_STRUCT_IDENT IDENT_STRUCT_humidity sensor
#define ROOT_STRUCT_TYPE struct humidity sensor
#define ROOT_STRUCT_TYPE_PTR ROOT_STRUCT_TYPE *
#define MAX_DEPTH 3

enum accessor_type {
    accessor_type_read,
    accessor_type_write,
    accessor_type_scan,
    accessor_last
};

// protocol status codes
enum status_code {
    status_code_ok,
    status_code_out_of_bounds,
    status_code_write_protected,
    status_code_invalid_accessor,
    status_code_invalid_type,
    status_code_broken_ptr,
    status_code_invalid_length,
    status_code_length_overflow,
    status_code_length_mismatch,
    status_code_invalid_opcode,
    status_code_broken_packet = 11,
    status_code_need_sync,
    status_code_no_trx,
    status_code_timeout
};

/* From the pointer treeref of type ident, access the given member into value
 * and value_type. */
void accessor(void *treeref, uint8_t ident, uint8_t member, enum accessor_type access,
              void **value, uint8_t *value_type, enum status_code *status) {
    if (!treeref)
        goto broken_ptr;

    *status = status_code_ok;
    switch (ident) {
    case IDENT_LIST_char: // accessing list [char]
        switch (member) {
        case (ACCESSOR_LIST_GENERIC_capacity):
            if (access == accessor_type_write)
                *status = status_code_write_protected;
            treeref = (void *) &((struct string *) treeref)->capacity;
            ident = IDENT_uint16_t;
            break;
        case (ACCESSOR_LIST_GENERIC_length):
            treeref = (void *) &((struct string *) treeref)->length;
            ident = IDENT_uint16_t;
            break;
        case (ACCESSOR_LIST_GENERIC_type):
            ident = IDENT_char;
            break;
        default:
            if (member - 3 >= ((struct string *) treeref)->length) {
                *status = status_code_out_of_bounds;
                return;
            }
            treeref = (void *) &((struct string *) treeref)->list[member - 3];
            ident = IDENT_char;
        }
        break;
    case IDENT_STRUCT_wireless_settings: // accessing struct wireless_settings
        switch (member) {
        case ACCESSOR_STRUCT_wireless_settings_network_ssid:
            treeref = (void *) ((struct wireless_settings *) treeref)->network_ssid;
            ident = IDENT_LIST_char;
            break;
        case ACCESSOR_STRUCT_wireless_settings_network_password:
            treeref = (void *) ((struct wireless_settings *) treeref)->network_password;
            ident = IDENT_LIST_char;
            break;
        default:
            goto broken_accessor;
        }
        break;
    
    case IDENT_STRUCT_polling_settings: // accessing struct polling_settings
        switch (member) {
        case ACCESSOR_STRUCT_polling_settings_poll_interval_mins:
            treeref = (void *) &((struct polling_settings *) treeref)->poll_interval_mins;
            ident = IDENT_uint16_t;
            break;
        default:
            goto broken_accessor;
        }
        break;
    
    case IDENT_STRUCT_humidity sensor: // accessing struct humidity sensor
        switch (member) {
        case ACCESSOR_STRUCT_humidity sensor_wireless_settings:
            treeref = (void *) ((struct humidity sensor *) treeref)->wireless_settings;
            ident = IDENT_STRUCT_wireless_settings;
            break;
        case ACCESSOR_STRUCT_humidity sensor_sensor_settings:
            treeref = (void *) ((struct humidity sensor *) treeref)->sensor_settings;
            ident = IDENT_STRUCT_polling_settings;
            break;
        default:
            goto broken_accessor;
        }
        break;
    
    default:
        goto broken_ident_type;
    }
    *value = treeref;
    *value_type = ident;
    return;

broken_accessor:
    *status = status_code_invalid_accessor;
    return;
broken_ident_type:
    *status = status_code_invalid_type;
    return;
broken_ptr:
    *status = status_code_broken_ptr;
    return;
}

void accessor_path(uint8_t *accessor_path, uint8_t accessor_path_len,
                   enum accessor_type access_type, void **value,
                   uint8_t *value_size, enum status_code *code)
{
    void *finger = ROOT_STRUCT_REF;
    uint8_t finger_type = ROOT_STRUCT_IDENT;

    for (uint8_t i = 0; i < accessor_path_len; i++) {
        accessor(finger, finger_type, accessor_path[i], access_type, &finger, &finger_type,
                 code);
        if (*code)
            return;
    }
    *value = finger;
    *value_size = ident_sizes[finger_type];

    if (!ident_is_primitive(finger_type))
        *code = status_code_write_protected;
}

struct iter_ctx {
    void *treeref;         // the containing object
    uint8_t refident;      // what kind is it??
    uint8_t accessor;      // the next member to access
    bool primitives_setup; // whether we've gotten through the primitives or not, which come first in a struct iteration
    uint8_t index;         // the array index
};

struct accessor_ctx {
    void *value_ptr;       // the member being iterated
    uint8_t value_ident;   // its type id
    uint8_t value_len;     // its length
    uint8_t depth;         // the depth in the tree
    uint8_t offset;        // its byte offset (from the total, pre-order serialization)
    uint8_t accessor;
};

struct iter_ctx *setup_iter(struct iter_ctx *ctx, void *treeref,
                            uint8_t refident) {
    ctx->treeref = treeref;
    ctx->refident = refident;
    ctx->accessor = 0;
    ctx->primitives_setup = false;
    ctx->index = 0;
    return ctx;
}

struct accessor_ctx *setup_accessor_ctx(struct accessor_ctx *acc_ctx,
                                        struct iter_ctx *it_ctx) {
    acc_ctx->value_ptr = it_ctx->treeref;
    acc_ctx->value_ident = it_ctx->refident;
    acc_ctx->value_len = ident_sizes[it_ctx->refident];
    acc_ctx->depth = 0;
    acc_ctx->offset = 0;
    acc_ctx->accessor = 0;
    return acc_ctx;
}

void scan_iter_next(struct iter_ctx **iterptr, struct accessor_ctx *accptr,
                    struct iter_ctx *ctxs) {
    struct iter_ctx *ctx = *iterptr;
    enum status_code code = status_code_ok;

    void *value_ptr;
    uint8_t value_ident;
    uint8_t acc;

    do {
        accessor(ctx->treeref, ctx->refident, ctx->accessor, accessor_type_scan,
                 &value_ptr, &value_ident, &code);

        // POST: value_ptr and value_ident point to the correct member for accessor
        // or code is set to indicate a boundary condition
        if (code == status_code_invalid_accessor ||
            code == status_code_invalid_type ||
            code == status_code_out_of_bounds) {
            if (!ctx->primitives_setup) {
                // start over for collection types
                ctx->primitives_setup = true;
                ctx->accessor = 0;
            } else if (ctx->index) {
                // pop context and head up to parents
                ctx = *iterptr = &ctxs[ctx->index - 1];
            } else {
                // finished root struct -- no parents
                *iterptr = NULL;
                return;
            }
        } else {
            // successful access, position for next one
            acc = ctx->accessor;
            ctx->accessor++;
            if (ctx->primitives_setup && !ident_is_primitive(value_ident)) {
                // issue pointer to collection, and set up iterptr to descend for next time
                uint8_t index = ctx->index + 1;
                ctx = *iterptr = &ctxs[index];
                setup_iter(ctx, value_ptr, value_ident);
                ctx->index = index;
                // printf("Bump found collection of type %hhu at member %hhu\n", value_ident, ctx->accessor - 1);
                break;
            } else if (!ctx->primitives_setup && ident_is_primitive(value_ident) && code != status_code_write_protected) {
                // printf("Bump found primitive of type %hhu at member %hhu\n", value_ident, ctx->accessor - 1);
                break;
            }
        }
    } while (1);

    accptr->value_ptr = value_ptr;
    accptr->value_ident = value_ident;
    accptr->value_len = ident_sizes[value_ident];
    accptr->depth = ident_is_primitive(value_ident) ? ctx->index + 1 : ctx->index;
    accptr->offset = ident_is_primitive(value_ident) ? accptr->offset + accptr->value_len : accptr->offset;
    accptr->accessor = acc;
}

// safely unpack a macro tag for single re-use in the following for... loop
#define iter_init(tag, init, type)    for (type tag = (init); tag; tag = NULL)

#define accessor_foreach(tag, treeref, refident, iter_ctxs)                         \
        iter_init(_iter_ctx, setup_iter(iter_ctxs, treeref, refident),              \
                  struct iter_ctx *)                                                \
        iter_init(__tag, setup_accessor_ctx(tag, _iter_ctx), struct accessor_ctx *) \
        for (; _iter_ctx; scan_iter_next(&_iter_ctx, __tag, iter_ctxs))

/* DEBUG library */
void print_bytes(void *ptr, int bytes) {
    // printf("(printing %d bytes...) ", bytes);
    uint8_t *byte_ptr = (uint8_t *) ptr;
    for (int i = 0; i < bytes; i++) {
        Serial.print(byte_ptr[i], DEC);
    }
    Serial.println("");
}

void print_length_field(uint8_t *buffer, uint8_t buflen) {
    for (int i = 0; i < buflen; i++) {
        Serial.print(buffer[i], DEC);
    }
    Serial.print(" ");
}

void print_accessor(uint8_t *accessor, uint8_t len) {
    Serial.print("Accessor: ");
    for (int i = 0; i < len; i++) {
        Serial.print(accessor[i], DEC);
    }
    Serial.println("");
}

void store_bytes(uint8_t *store, uint8_t *tree, int numbytes) {
    for (int i = 0; i < numbytes; i++)
        store[i] = tree[i];
}

void dump_config(void *treeref, enum ident_type ident) {
    struct iter_ctx iter_ctxs[MAX_DEPTH];

    struct accessor_ctx acc_ctx_inst;
    struct accessor_ctx *acc_ctx = &acc_ctx_inst;

    accessor_foreach(acc_ctx, treeref, ident, iter_ctxs) {
        for (int i = 0; i < acc_ctx->depth - 1; i++)
            Serial.print("     ");
        if (acc_ctx->depth)
            Serial.print("   ");
            Serial.print(acc_ctx->accessor, DEC);
            Serial.print(": ");
        if (ident_is_primitive(acc_ctx->value_ident)) {
            Serial.print("Primitive {");
            Serial.print(ident_labels[acc_ctx->value_ident]);
            Serial.print("id: ");
            Serial.print(acc_ctx->value_ident, DEC);
            Serial.print(" length: ");
            Serial.print(acc_ctx->value_len, DEC);
            Serial.print(" offset: ");
            Serial.print(acc_ctx->offset - acc_ctx->value_len, DEC);
            Serial.print(":");
            Serial.print(acc_ctx->offset, DEC);
            Serial.print("} ");
            print_bytes(acc_ctx->value_ptr, acc_ctx->value_len);
        } else if (ident_is_list(acc_ctx->value_ident)) {
            Serial.print("List {");
            Serial.print(ident_labels[acc_ctx->value_ident]);
            Serial.print(", id: ");
            Serial.print(acc_ctx->value_ident, DEC);
            Serial.println("}");
        } else {
            Serial.print("Struct {");
            Serial.print(ident_labels[acc_ctx->value_ident]);
            Serial.print(", id: ");
            Serial.print(acc_ctx->value_ident, DEC);
            Serial.println("}");
        }
    }
}

/* STORAGE */
void load_config_byte(uint8_t *ptr, uint16_t addr, uint8_t i) {
    ptr[i] = EEPROM.read(addr + i);
}

void store_config_byte(uint8_t *ptr, uint16_t addr, uint8_t i) {
    EEPROM.write(addr + i, ptr[i]);
}

void scan_config(void (*value_fn)(uint8_t *, uint16_t, uint8_t), uint16_t offset) {
    struct iter_ctx iter_ctxs[MAX_DEPTH];

    struct accessor_ctx acc_ctx_inst;
    struct accessor_ctx *acc_ctx = &acc_ctx_inst;

    uint16_t address = offset;
    accessor_foreach(acc_ctx, ROOT_STRUCT_REF, ROOT_STRUCT_IDENT, iter_ctxs) {
        if (ident_is_primitive(acc_ctx->value_ident)) {
            uint8_t *ptr = (uint8_t *) acc_ctx->value_ptr;
            for (int i = 0; i < acc_ctx->value_len; i++)
                (*value_fn)(ptr, address, i);
            address += acc_ctx->value_len;
        }
    }
}

/* PROTOCOL */
enum opcode {
    opcode_read = 1,
    opcode_write,
    opcode_describe,
    opcode_marco,
    opcode_open_trx,
    opcode_hold,
    opcode_commit_trx,
    opcode_cancel_trx,
    opcode_trx_id,
    opcode_need_sync = 11,
    opcode_debug,
    opcode_invalid,
    opcode_last
};

void proto_print(String str) {
    if (proto_debug) {
        Serial.write(opcode_debug);
        Serial.write((uint8_t) 0);
        Serial.write((uint8_t) str.length());
        Serial.print(str);
        Serial.print('\n');
        Serial.flush();
    }
}

const int timeout = 5000;

bool timed_out(int start, bool hold) {
    return !hold && millis() - start >= timeout; //  || millis() < start;
}

bool wait_for_protocol_header(bool hold) {
    int start = millis();
    while (Serial.available() < 3 && !timed_out(start, hold))
        ;
    return timed_out(start, hold);
}

bool parse_length_field(uint8_t *buffer, int bytes, bool hold) {
    int i = 0, start = millis();
    while (i < bytes && !timed_out(start, hold))
        if (Serial.peek() != -1)
            buffer[i++] = Serial.read();
    return timed_out(start, hold);
}

void put_length_field(uint8_t *buffer, int buflen) {
    Serial.write(buflen);
    for (int i = 0; i < buflen; i++) {
        Serial.write(buffer[i]);
    }
}

bool scan_through_newline() {
    int c, start = millis();
    while ((c = Serial.peek()) != '\n' && !timed_out(start, false));
        Serial.read();

    Serial.read();
    return timed_out(start, false);
}

#define SYNC_BYTES 600
bool handle_sync() {
    proto_print("Handling sync...");
    int start = millis();

    for (int i = 0; i < SYNC_BYTES * 2; i++)
        Serial.write('\0');
    Serial.print('\n');
    Serial.flush();

    do {
        if (timed_out(start, false))
            return true;

        // scan to find region of 0s
        while (Serial.read() != '\0' && !timed_out(start, false))
            ;

        // scan and count, until we find MAX_BYTES of 0s
        int i;
        for (i = 0; i < SYNC_BYTES; i++)
            if ((c = Serial.read()) != '\0')
                break;
        if (i == SYNC_BYTES)
            break;
    } while (1);

    return scan_through_newline();
}

void handle_protocol(uint8_t *trx_id) {
    int opcode;
    enum status_code code;
    bool hold = false;
    uint8_t trx_start = *trx_id;

    int slot_1_len, slot_2_len;
    uint8_t slot_1[100], slot_2[100];

    void *result_slot;
    uint8_t result_len;

    while (1) {
        code = status_code_ok;
        result_len = 0;

        proto_print("Protocol loop");
        if (wait_for_protocol_header(hold)) {
            *trx_id = trx_start;
            return;
        }
        opcode = Serial.read();
        proto_print("Have opcode");

        slot_1_len = Serial.read();
        slot_2_len = Serial.read();
        if (slot_1_len > 100 || slot_2_len > 100) {
            code = status_code_length_mismatch;
            opcode = opcode_need_sync;
        } else if (parse_length_field(slot_1, slot_1_len, hold) ||
                   parse_length_field(slot_2, slot_2_len, hold) ||
                   scan_through_newline()) {
            *trx_id = trx_start;
            return;
        }

        proto_print("Have payloads");
        switch(opcode) {
        case opcode_read:
            proto_print("Opcode read");
            accessor_path(slot_1, slot_1_len,
                          accessor_type_read,
                          &result_slot, &result_len, &code);
            break;
        case opcode_write:
            proto_print("Opcode write");
            void *treeref;
            uint8_t treelen;
            accessor_path(slot_1, slot_1_len,
                          accessor_type_write,
                          &treeref, &treelen, &code);
            if (slot_2_len != treelen)
                code = status_code_length_mismatch;
            if (code)
                break;

            store_bytes((uint8_t *) treeref, slot_2, slot_2_len);
            break;
        case opcode_marco:
            proto_print("Opcode marco");
            result_slot = slot_1;
            result_len = slot_1_len;
            break;
        case opcode_hold:
            proto_print("Hold issued");
            hold = !!slot_1[0];
            break;
        case opcode_cancel_trx:
            proto_print("Opcode cancel");
            *trx_id = trx_start;
            break;
        case opcode_commit_trx:
            proto_print("Opcode commit trx");
            if (slot_1_len == 0 || slot_1[0] != *trx_id) {
                code = status_code_broken_packet;
                proto_print("Commit failure");
            } else {
                proto_print("Commit success");
                (*trx_id)++;
                result_slot = trx_id;
                result_len = 1;
            }
            break;
        case opcode_trx_id:
            proto_print("Trx id read");
            result_slot = trx_id;
            result_len = 1;
            break;
        default:
            code = status_code_need_sync;
            break;
        }

        proto_print("Writing response");
        Serial.write(opcode);
        Serial.write(code);
        put_length_field((uint8_t *) result_slot, result_len);
        Serial.print('\n');
        Serial.flush();

        if (code != status_code_ok) {
            if (handle_sync()) {
                *trx_id = trx_start;
                return;
            }
        } else if (opcode == opcode_commit_trx || opcode == opcode_cancel_trx) {
            return;
        }
    }
}

/* INTERFACE AND STATE */
uint8_t cur_slot;
uint8_t read_slot;
uint8_t trx_id;
uint16_t config_size;

void init_config() {
    if (!EEPROM.isValid()) {
        proto_print("Setting up eeprom");
        cur_slot = read_slot = 0;
        trx_id = 0;
        scan_config(&store_config_byte, 2);
        EEPROM.commit();
    } else {
        proto_print("Reading from eeprom");
        cur_slot = read_slot = EEPROM.read(0);
        if (cur_slot >= slots)
            cur_slot = 0;
        trx_id = EEPROM.read(1);
        scan_config(&load_config_byte, 2 + config_size * cur_slot);
    }

    struct iter_ctx iter_ctxs[MAX_DEPTH];

    struct accessor_ctx acc_ctx_inst;
    struct accessor_ctx *acc_ctx = &acc_ctx_inst;

    uint16_t address = 0;
    accessor_foreach(acc_ctx, ROOT_STRUCT_REF, ROOT_STRUCT_IDENT, iter_ctxs) {
        address += acc_ctx->value_len;
    }
    config_size = address;
}

ROOT_STRUCT_TYPE_PTR read_config() {
    if (cur_slot != read_slot) {
        scan_config(&load_config_byte, 2 + config_size * read_slot);
        cur_slot = read_slot;
    }
    return ROOT_STRUCT_REF;
}

uint8_t current_slot() {
    return cur_slot;
}

void set_slot(uint8_t slot) {
    cur_slot = slot;
}

bool handle_serial_update() {
    uint8_t start_trx = trx_id;
    handle_protocol(&trx_id);
    if (trx_id != start_trx) {
        proto_print("new version: committing");
        scan_config(&store_config_byte, 2 + config_size * cur_slot);
        EEPROM.commit();
    } else {
        proto_print("same version: reverting");
        scan_config(&load_config_byte, 2 + config_size * cur_slot);
    }
    return trx_id != start_trx;
}
