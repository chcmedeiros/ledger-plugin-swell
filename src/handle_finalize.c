#include "swell_plugin.h"

void handle_finalize(ethPluginFinalize_t *msg) {
    context_t *context = (context_t *) msg->pluginContext;

    msg->uiType = ETH_UI_TYPE_GENERIC;

    // The total number of screen you will need.
    switch (context->selectorIndex) {
        case ADD_NEW_VALIDATOR:
            msg->numScreens = context->tx.body.add_new_validator.n_pubkeys;
            break;
        case ADD_OPERATOR:
            msg->numScreens = 3;
            break;
        case DELETE_ACTIVE_VALIDATORS:
        case DELETE_PENDING_VALIDATORS:
        case USE_PUBKEYS_FOR_VALIDATOR:
            msg->numScreens = context->tx.body.pubkey_methods.n_pubkeys;
            break;
        case DISABLE_OPERATOR:
        case ENABLE_OPERATOR:
        case INITIALIZE:
        case WITHDRAWERC20:
        case DEPOSIT:
            msg->numScreens = 1;
            break;
        case UPDATE_OPERATOR_ADDRESS:
        case UPDATE_OPERATOR_NAME:
        case UPDATE_OPERATOR_REWARD:
            msg->numScreens = 2;
            break;
        default:
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            return;
    }

    msg->result = ETH_PLUGIN_RESULT_OK;
}
