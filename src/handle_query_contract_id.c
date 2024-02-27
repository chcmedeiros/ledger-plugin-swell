#include "swell_plugin.h"

// Sets the first screen to display.
void handle_query_contract_id(ethQueryContractID_t *msg) {
    const context_t *context = (const context_t *) msg->pluginContext;
    // msg->name will be the upper sentence displayed on the screen.
    // msg->version will be the lower sentence displayed on the screen.

    // For the first screen, display the plugin name.
    strlcpy(msg->name, PLUGIN_NAME, msg->nameLength);

    switch (context->selectorIndex) {
        case ADD_NEW_VALIDATOR:
            strlcpy(msg->version, "Add New Validator Details", msg->versionLength);
            break;
        case ADD_OPERATOR:
            strlcpy(msg->version, "Add Operator", msg->versionLength);
            break;
        case DELETE_ACTIVE_VALIDATORS:
            strlcpy(msg->version, "Delete Active Validators", msg->versionLength);
            break;
        case DELETE_PENDING_VALIDATORS:
            strlcpy(msg->version, "Delete Pending Validators", msg->versionLength);
            break;
        case DISABLE_OPERATOR:
            strlcpy(msg->version, "Disable Operator", msg->versionLength);
            break;
        case ENABLE_OPERATOR:
            strlcpy(msg->version, "Enable Operator", msg->versionLength);
            break;
        case INITIALIZE:
            strlcpy(msg->version, "Initialize", msg->versionLength);
            break;
        case UPDATE_OPERATOR_ADDRESS:
            strlcpy(msg->version, "Update Operator Address", msg->versionLength);
            break;
        case UPDATE_OPERATOR_NAME:
            strlcpy(msg->version, "Update Operator Name", msg->versionLength);
            break;
        case UPDATE_OPERATOR_REWARD:
            strlcpy(msg->version, "Update Operator Reward", msg->versionLength);
            break;
        case USE_PUBKEYS_FOR_VALIDATOR:
            strlcpy(msg->version, "Use Pubkeys For Validators", msg->versionLength);
            break;
        case WITHDRAWERC20:
            strlcpy(msg->version, "WithdrawERC20", msg->versionLength);
            break;
        case DEPOSIT:
            strlcpy(msg->version, "Deposit", msg->versionLength);
            break;
        default:
            PRINTF("Selector index: %d not supported\n", context->selectorIndex);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            return;
    }

    msg->result = ETH_PLUGIN_RESULT_OK;
}
