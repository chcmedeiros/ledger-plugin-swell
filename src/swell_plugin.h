#pragma once

#include "eth_plugin_interface.h"
#include <string.h>

// Number of selectors defined in this plugin. Should match the enum `selector_t`.
#define NUM_SELECTORS 13

// Name of the plugin.
#define PLUGIN_NAME "Swell"

#define HALF_PARAMETER_LENGTH 16

// Enumeration of the different selectors possible.
// Should follow the exact same order as the array declared in main.c
typedef enum {
    ADD_NEW_VALIDATOR = 0,
    ADD_OPERATOR,
    DELETE_ACTIVE_VALIDATORS,
    DELETE_PENDING_VALIDATORS,
    DISABLE_OPERATOR,
    ENABLE_OPERATOR,
    INITIALIZE,
    UPDATE_OPERATOR_ADDRESS,
    UPDATE_OPERATOR_NAME,
    UPDATE_OPERATOR_REWARD,
    USE_PUBKEYS_FOR_VALIDATOR,
    WITHDRAWERC20,
    DEPOSIT
} selector_t;

// Enumeration used to parse the smart contract data.
typedef enum {
    ADDRESS = 0,
    NAME,
    NAME_OFFSET,
    OFFSET,
    NAME_LEN,
    NAME_OFFSET_1,
    NAME_OFFSET_2,
    OPERATOR,
    REWARD,
    PUBKEY,
    PUBKEY_OFFSET,
    PUBKEY_LEN,
    PUBKEY_OFFSET_1,
    PUBKEY_OFFSET_2,
    N_PUBKEYS,
    TUPLE_OFFSET_1,
    OFFSETS,
    NONE
} parameter;

extern const uint32_t SWELL_SELECTORS[NUM_SELECTORS];

typedef struct {
    uint8_t value[ADDRESS_LENGTH];
} address_t;

typedef struct {
    uint16_t len;
    uint8_t text[PARAMETER_LENGTH + 1];
    bool ellipsis;
} name_t;

typedef struct {
    uint8_t value[INT256_LENGTH];
    bool ellipsis;
} bytes32_t;

typedef struct {
    union {
        struct {
            uint16_t n_pubkeys;
            uint16_t pubkeys_len[4];
            bytes32_t pubkey[4];
            uint8_t id;
            uint16_t tuple_offsets[4];
            uint16_t tuple_offsets_start;
        } add_new_validator;

        struct {
            uint16_t n_pubkeys;
            uint16_t pubkeys_len[4];
            bytes32_t pubkey[4];
            uint8_t id;
            uint16_t offsets[4];
            uint16_t offsets_start;
        } pubkey_methods;

        struct {
            name_t name;
            address_t operator;
            address_t reward;
            uint16_t name_offset;
        } add_operator;

        struct {
            address_t operator;
        } disable_enable_operator;

        struct {
            address_t control;
        } initialize;
        struct {
            address_t operator;
            address_t new_operator;
        } update_operator_address;
        struct {
            address_t operator;
            name_t name;
        } update_operator_name;

        struct {
            address_t operator;
            address_t reward;
        } update_operator_reward;

        struct {
            address_t token_addr;
        } withdrawerc20;

    } body;
} swell_tx_t;

// Shared global memory with Ethereum app. Must be at most 5 * 32 bytes.
typedef struct context_t {
    // For parsing data.
    uint8_t next_param;  // Set to be the next param we expect to parse.
    uint16_t offset;     // Offset at which the array or struct starts.
    bool go_to_offset;   // If set, will force the parsing to iterate through parameters until
                         // `offset` is reached.

    // For both parsing and display.
    selector_t selectorIndex;

    // Tx related context
    swell_tx_t tx;
} context_t;

// Piece of code that will check that the above structure is not bigger than 5 * 32. Do not remove
// this check.
#ifndef BUILDING_FUZZER
ASSERT_SIZEOF_PLUGIN_CONTEXT(context_t);
#endif
