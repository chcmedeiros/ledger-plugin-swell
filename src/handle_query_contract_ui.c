#include <stdbool.h>
#include "swell_plugin.h"

// Set UI for any address screen.
static bool set_address_ui(ethQueryContractUI_t *msg, address_t *value) {
    // Prefix the address with `0x`.
    msg->msg[0] = '0';
    msg->msg[1] = 'x';

    // We need a random chainID for legacy reasons with `getEthAddressStringFromBinary`.
    // Setting it to `0` will make it work with every chainID :)
    uint64_t chainid = 0;

    // Get the string representation of the address stored in `context->beneficiary`. Put it in
    // `msg->msg`.
    return getEthAddressStringFromBinary(
        value->value,
        msg->msg + 2,  // +2 here because we've already prefixed with '0x'.
        msg->pluginSharedRW->sha3,
        chainid);
}

static bool array_to_hexstr(char *dst,
                            size_t dstLen,
                            const uint8_t *src,
                            uint16_t count,
                            bool addEllipsis) {
    memset(dst, 0, dstLen);
    if (dstLen < (count * 2 + 1)) {
        return false;
    }

    const char hexchars[] = "0123456789abcdef";
    int halfCount = count / 2;  // Calculate the middle point

    for (int i = 0; i < halfCount; i++, src++) {
        *dst++ = hexchars[*src >> 4u];
        *dst++ = hexchars[*src & 0x0Fu];
    }

    if (addEllipsis) {
        // Add "..." in the middle
        *dst++ = '.';
        *dst++ = '.';
        *dst++ = '.';
    }

    for (int i = halfCount; i < count; i++, src++) {
        *dst++ = hexchars[*src >> 4u];
        *dst++ = hexchars[*src & 0x0Fu];
    }

    *dst = 0;  // terminate string

    return true;
}

static bool set_bytes_ui(ethQueryContractUI_t *msg,
                         bytes32_t *array,
                         uint16_t src_len,
                         const char *title) {
    strlcpy(msg->title, title, msg->titleLength);

    return array_to_hexstr(msg->msg,
                           msg->msgLength,
                           array->value,
                           (src_len > PARAMETER_LENGTH) ? PARAMETER_LENGTH : src_len,
                           array->ellipsis);
}

static bool set_addr_ui(ethQueryContractUI_t *msg, address_t *address, const char *title) {
    strlcpy(msg->title, title, msg->titleLength);
    return set_address_ui(msg, address);
}

static bool set_name_ui(ethQueryContractUI_t *msg, name_t *name, const char *title) {
    strlcpy(msg->title, title, msg->titleLength);
    if (name->ellipsis) {
        snprintf(msg->msg, msg->msgLength, "%.*s...%s", 16, name->text, name->text + 16);
        return true;
    } else {
        snprintf(msg->msg, msg->msgLength, "%s", name->text);
        return true;
    }
    return false;
}

void handle_query_contract_ui(ethQueryContractUI_t *msg) {
    context_t *context = (context_t *) msg->pluginContext;
    bool ret = false;
    // msg->title is the upper line displayed on the device.
    // msg->msg is the lower line displayed on the device.

    // Clean the display fields.
    memset(msg->title, 0, msg->titleLength);
    memset(msg->msg, 0, msg->msgLength);

    switch (context->selectorIndex) {
        case ADD_NEW_VALIDATOR:
            if (msg->screenIndex < context->tx.body.add_new_validator.n_pubkeys) {
                ret = set_bytes_ui(msg,
                                   &context->tx.body.add_new_validator.pubkey[msg->screenIndex],
                                   context->tx.body.add_new_validator.pubkeys_len[msg->screenIndex],
                                   "Pubkey");
            } else {
                PRINTF("Received an invalid screenIndex\n");
                ret = false;
            }
            break;
        case ADD_OPERATOR:
            switch (msg->screenIndex) {
                case 0:
                    ret = set_name_ui(msg, &context->tx.body.add_operator.name, "Name");
                    break;
                case 1:
                    ret =
                        set_addr_ui(msg, &context->tx.body.add_operator.operator, "Operator Addr");
                    break;
                case 2:
                    ret = set_addr_ui(msg, &context->tx.body.add_operator.reward, "Reward Addr");
                    break;
                default:
                    PRINTF("Received an invalid screenIndex\n");
                    ret = false;
            }
            break;
        case DELETE_ACTIVE_VALIDATORS:
        case DELETE_PENDING_VALIDATORS:
        case USE_PUBKEYS_FOR_VALIDATOR:
            if (msg->screenIndex < context->tx.body.pubkey_methods.n_pubkeys) {
                ret = set_bytes_ui(msg,
                                   &context->tx.body.pubkey_methods.pubkey[msg->screenIndex],
                                   context->tx.body.pubkey_methods.pubkeys_len[msg->screenIndex],
                                   "Pubkey");
            } else {
                PRINTF("Received an invalid screenIndex\n");
                ret = false;
            }
            break;
        case DISABLE_OPERATOR:
        case ENABLE_OPERATOR:
            if (msg->screenIndex == 0) {
                ret = set_addr_ui(msg,
                                  &context->tx.body.disable_enable_operator.operator,
                                  "Operator Addr");
            } else {
                PRINTF("Received an invalid screenIndex\n");
                ret = false;
            }
            break;
        case INITIALIZE:
            if (msg->screenIndex == 0) {
                ret = set_addr_ui(msg, &context->tx.body.initialize.control, "Control Manager");
            } else {
                PRINTF("Received an invalid screenIndex\n");
                ret = false;
            }
            break;
        case UPDATE_OPERATOR_ADDRESS:
            switch (msg->screenIndex) {
                case 0:
                    ret = set_addr_ui(msg,
                                      &context->tx.body.update_operator_address.operator,
                                      "Operator Addr");
                    break;
                case 1:
                    ret = set_addr_ui(msg,
                                      &context->tx.body.update_operator_address.new_operator,
                                      "New Operator Addr");
                    break;
                default:
                    PRINTF("Received an invalid screenIndex\n");
                    ret = false;
            }
            break;
        case UPDATE_OPERATOR_NAME:
            switch (msg->screenIndex) {
                case 0:
                    ret = set_addr_ui(msg,
                                      &context->tx.body.update_operator_name.operator,
                                      "Operator Addr");
                    break;
                case 1:
                    ret = set_name_ui(msg, &context->tx.body.update_operator_name.name, "Name");
                    break;
                default:
                    PRINTF("Received an invalid screenIndex\n");
                    ret = false;
            }
            break;
        case UPDATE_OPERATOR_REWARD:
            switch (msg->screenIndex) {
                case 0:
                    ret = set_addr_ui(msg,
                                      &context->tx.body.update_operator_reward.operator,
                                      "Operator Addr");
                    break;
                case 1:
                    ret = set_addr_ui(msg,
                                      &context->tx.body.update_operator_reward.reward,
                                      "Reward Addr");
                    break;
                default:
                    PRINTF("Received an invalid screenIndex\n");
                    ret = false;
            }
            break;
        case WITHDRAWERC20:
            if (msg->screenIndex == 0) {
                ret = set_addr_ui(msg, &context->tx.body.withdrawerc20.token_addr, "Token Addr");
            } else {
                PRINTF("Received an invalid screenIndex\n");
                ret = false;
            }
            break;
        case DEPOSIT:
            if (msg->screenIndex == 0) {
                strlcpy(msg->title, "Stake", msg->titleLength);
                const uint8_t *eth_amount = msg->pluginSharedRO->txContent->value.value;
                uint8_t eth_amount_size = msg->pluginSharedRO->txContent->value.length;

                ret = amountToString(eth_amount,
                                     eth_amount_size,
                                     WEI_TO_ETHER,
                                     "ETH",
                                     msg->msg,
                                     msg->msgLength);
            } else {
                PRINTF("Received an invalid screenIndex\n");
                ret = false;
            }
            break;

        default:
            PRINTF("Selector index: %d not supported\n", context->selectorIndex);
            ret = false;
    }
    msg->result = ret ? ETH_PLUGIN_RESULT_OK : ETH_PLUGIN_RESULT_ERROR;
}
