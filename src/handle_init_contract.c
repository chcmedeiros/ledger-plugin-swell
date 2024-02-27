#include "swell_plugin.h"

static bool find_sel(uint32_t selector, const uint32_t *selectors, size_t n, selector_t *out) {
    for (selector_t i = 0; i < n; i++) {
        if (selector == selectors[i]) {
            *out = i;
            return true;
        }
    }
    return false;
}

// Called once to init.
void handle_init_contract(ethPluginInitContract_t *msg) {
    // Make sure we are running a compatible version.
    if (msg->interfaceVersion != ETH_PLUGIN_INTERFACE_VERSION_LATEST) {
        // If not the case, return the `UNAVAILABLE` status.
        msg->result = ETH_PLUGIN_RESULT_UNAVAILABLE;
        return;
    }

    // Double check that the `context_t` struct is not bigger than the maximum size (defined by
    // `msg->pluginContextLength`).
    if (msg->pluginContextLength < sizeof(context_t)) {
        msg->result = ETH_PLUGIN_RESULT_ERROR;
        return;
    }
    context_t *context = (context_t *) msg->pluginContext;

    // Initialize the context (to 0).
    memset(context, 0, sizeof(*context));

    uint32_t selector = U4BE(msg->selector, 0);
    if (!find_sel(selector, SWELL_SELECTORS, NUM_SELECTORS, &context->selectorIndex)) {
        PRINTF("Error: selector not found!\n");
        msg->result = ETH_PLUGIN_RESULT_UNAVAILABLE;
        return;
    }

    // Set `next_param` to be the first field we expect to parse.
    switch (context->selectorIndex) {
        case ADD_OPERATOR:
            context->next_param = NAME_OFFSET;
            break;
        case DELETE_ACTIVE_VALIDATORS:
        case DELETE_PENDING_VALIDATORS:
        case USE_PUBKEYS_FOR_VALIDATOR:
        case ADD_NEW_VALIDATOR:
            context->next_param = OFFSET;
            break;
        case DISABLE_OPERATOR:
        case ENABLE_OPERATOR:
        case INITIALIZE:
        case WITHDRAWERC20:
            context->next_param = ADDRESS;
            break;
        case UPDATE_OPERATOR_ADDRESS:
        case UPDATE_OPERATOR_NAME:
        case UPDATE_OPERATOR_REWARD:
            context->next_param = OPERATOR;
            break;
        case DEPOSIT:
            break;
        default:
            PRINTF("Missing selectorIndex: %d\n", context->selectorIndex);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            return;
    }

    // Return valid status.
    msg->result = ETH_PLUGIN_RESULT_OK;
}
