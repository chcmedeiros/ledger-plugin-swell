#include "swell_plugin.h"

// Define here the selectors
static const uint32_t ADD_NEW_VALIDATORS_DETAILS = 0x60ec5216;
static const uint32_t ADD_OPERATOR_SELECTOR = 0x54741d6d;
static const uint32_t DELETE_ACTIVE_VALIDATORS_SELECTOR = 0xeda74e71;
static const uint32_t DELETE_PENDING_VALIDATORS_SELECTOR = 0x242eba0e;
static const uint32_t DEPOSIT_SELECTOR = 0xd0e30db0;
static const uint32_t DISABLE_OPERATOR_SELECTOR = 0xf56408ed;
static const uint32_t ENABLE_OPERATOR_SELECTOR = 0xdd307b99;
static const uint32_t INITIALIZE_SELECTOR = 0xc4d66de8;
static const uint32_t UPDATE_OPERATOR_ADDRESS_SELECTOR = 0x32f73258;
static const uint32_t UPDATE_OPERATOR_NAME_SELECTOR = 0x9f5db69c;
static const uint32_t UPDATE_OPERATOR_REWARD_SELECTOR = 0xe8f28a6c;
static const uint32_t USE_PUBKEYS_FOR_VALIDATOR_SELECTOR = 0xc3953502;
static const uint32_t WITHDRAWERC20_SELECTOR = 0xf4f3b200;

// Array of all the different ens selectors. Make sure this follows the same order as the
// enum defined in `opensea_plugin.h`
const uint32_t SWELL_SELECTORS[NUM_SELECTORS] = {ADD_NEW_VALIDATORS_DETAILS,
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
                                                 WITHDRAWERC20_SELECTOR,
                                                 DEPOSIT_SELECTOR};
