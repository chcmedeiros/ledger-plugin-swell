#include "swell_plugin.h"

// Define here the selectors
static const uint8_t ADD_NEW_VALIDATORS_DETAILS[SELECTOR_SIZE] = {0x60, 0xec, 0x52, 0x16};
static const uint8_t ADD_OPERATOR_SELECTOR[SELECTOR_SIZE] = {0x54, 0x74, 0x1d, 0x6d};
static const uint8_t DELETE_ACTIVE_VALIDATORS_SELECTOR[SELECTOR_SIZE] = {0xed, 0xa7, 0x4e, 0x71};
static const uint8_t DELETE_PENDING_VALIDATORS_SELECTOR[SELECTOR_SIZE] = {0x24, 0x2e, 0xba, 0x0e};
static const uint8_t DISABLE_OPERATOR_SELECTOR[SELECTOR_SIZE] = {0xf5, 0x64, 0x08, 0xed};
static const uint8_t ENABLE_OPERATOR_SELECTOR[SELECTOR_SIZE] = {0xdd, 0x30, 0x7b, 0x99};
static const uint8_t INITIALIZE_SELECTOR[SELECTOR_SIZE] = {0xc4, 0xd6, 0x6d, 0xe8};
static const uint8_t UPDATE_OPERATOR_ADDRESS_SELECTOR[SELECTOR_SIZE] = {0x32, 0xf7, 0x32, 0x58};
static const uint8_t UPDATE_OPERATOR_NAME_SELECTOR[SELECTOR_SIZE] = {0x9f, 0x5d, 0xb6, 0x9c};
static const uint8_t UPDATE_OPERATOR_REWARD_SELECTOR[SELECTOR_SIZE] = {0xe8, 0xf2, 0x8a, 0x6c};
static const uint8_t USE_PUBKEYS_FOR_VALIDATOR_SELECTOR[SELECTOR_SIZE] = {0xc3, 0x95, 0x35, 0x02};
static const uint8_t WITHDRAWERC20_SELECTOR[SELECTOR_SIZE] = {0xf4, 0xf3, 0xb2, 0x00};

// Array of all the different ens selectors. Make sure this follows the same order as the
// enum defined in `opensea_plugin.h`
const uint8_t *const SWELL_SELECTORS[NUM_SELECTORS] = {ADD_NEW_VALIDATORS_DETAILS,
                                                       ADD_OPERATOR_SELECTOR,
                                                       DELETE_ACTIVE_VALIDATORS_SELECTOR,
                                                       DELETE_PENDING_VALIDATORS_SELECTOR,
                                                       DISABLE_OPERATOR_SELECTOR,
                                                       ENABLE_OPERATOR_SELECTOR,
                                                       INITIALIZE_SELECTOR,
                                                       UPDATE_OPERATOR_ADDRESS_SELECTOR,
                                                       UPDATE_OPERATOR_NAME_SELECTOR,
                                                       UPDATE_OPERATOR_REWARD_SELECTOR,
                                                       USE_PUBKEYS_FOR_VALIDATOR_SELECTOR,
                                                       WITHDRAWERC20_SELECTOR};
