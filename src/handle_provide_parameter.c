#include "swell_plugin.h"

uint8_t bytes_missing = 0;

static void copy_text(uint8_t *dst, uint16_t dst_len, uint16_t max_len, const uint8_t *src) {
    size_t len = MIN(dst_len, max_len);
    memcpy(dst, src, len);
}

static void handle_operator_name(ethPluginProvideParameter_t *msg, context_t *context) {
    uint16_t containers = 0;  // group of 32 bytes needed to hold name

    if (context->go_to_offset) {
        if (msg->parameterOffset != context->offset) {
            return;
        }
        context->go_to_offset = false;
    }

    switch (context->next_param) {
        case OPERATOR:
            copy_address(context->tx.body.update_operator_name.operator.value,
                         msg->parameter,
                         sizeof(context->tx.body.update_operator_name.operator.value));
            context->next_param = NAME_OFFSET;
            break;
        case NAME_OFFSET:
            context->next_param = NAME_LEN;
            break;
        case NAME_LEN:
            if (!U2BE_from_parameter(msg->parameter,
                                     &context->tx.body.update_operator_name.name.len)) {
                msg->result = ETH_PLUGIN_RESULT_ERROR;
                return;
            }
            context->next_param = NAME;
            break;
        case NAME:
            // Name has less then 32
            if (context->tx.body.update_operator_name.name.len <= PARAMETER_LENGTH) {
                copy_text(context->tx.body.update_operator_name.name.text,
                          context->tx.body.update_operator_name.name.len,
                          PARAMETER_LENGTH,
                          msg->parameter);
                context->tx.body.update_operator_name.name.ellipsis = false;
                context->next_param = NONE;
            } else {  // Name has more then 32 bytes
                context->tx.body.update_operator_name.name.ellipsis = true;
                containers = context->tx.body.update_operator_name.name.len / PARAMETER_LENGTH;
                bytes_missing = context->tx.body.update_operator_name.name.len % PARAMETER_LENGTH;

                // copy first 16 bytes
                copy_text(context->tx.body.update_operator_name.name.text,
                          context->tx.body.update_operator_name.name.len,
                          HALF_PARAMETER_LENGTH,
                          msg->parameter);

                if (bytes_missing < HALF_PARAMETER_LENGTH) {
                    if (containers < 2) {  // only one container we still need bytes from this one
                                           // to complete the last 16. then go to last container
                        copy_text(
                            context->tx.body.update_operator_name.name.text + HALF_PARAMETER_LENGTH,
                            context->tx.body.update_operator_name.name.len - HALF_PARAMETER_LENGTH,
                            HALF_PARAMETER_LENGTH - bytes_missing,
                            msg->parameter + HALF_PARAMETER_LENGTH + bytes_missing);

                        context->offset = msg->parameterOffset + PARAMETER_LENGTH;
                        context->go_to_offset = true;
                        context->next_param = NAME_OFFSET_2;
                    } else {  // more then 1 container go to second-last and get missing bytes to
                              // complete the last 16
                        context->offset =
                            msg->parameterOffset + (containers - 1) * PARAMETER_LENGTH;
                        context->go_to_offset = true;
                        context->next_param = NAME_OFFSET_1;
                    }
                } else {  // last container has the last 16 bytes we need
                    context->offset = msg->parameterOffset + containers * PARAMETER_LENGTH;
                    context->go_to_offset = true;
                    context->next_param = NAME_OFFSET_2;
                }
            }
            break;
        case NAME_OFFSET_1:  // second last container
            copy_text(context->tx.body.update_operator_name.name.text + HALF_PARAMETER_LENGTH,
                      context->tx.body.update_operator_name.name.len - HALF_PARAMETER_LENGTH,
                      HALF_PARAMETER_LENGTH - bytes_missing,
                      msg->parameter + HALF_PARAMETER_LENGTH + bytes_missing);
            context->next_param = NAME_OFFSET_2;
            break;
        case NAME_OFFSET_2:                                // last container
            if (bytes_missing <= HALF_PARAMETER_LENGTH) {  // copy missing bytes
                copy_text(context->tx.body.update_operator_name.name.text + HALF_PARAMETER_LENGTH +
                              (HALF_PARAMETER_LENGTH - bytes_missing),
                          context->tx.body.update_operator_name.name.len - HALF_PARAMETER_LENGTH +
                              (HALF_PARAMETER_LENGTH - bytes_missing),
                          bytes_missing,
                          msg->parameter);
            } else {  // last container has 16 or more bytes, move the need offset to copy the last
                      // 16 bytes
                copy_text(context->tx.body.update_operator_name.name.text + HALF_PARAMETER_LENGTH,
                          context->tx.body.update_operator_name.name.len - HALF_PARAMETER_LENGTH,
                          HALF_PARAMETER_LENGTH,
                          msg->parameter + (bytes_missing - HALF_PARAMETER_LENGTH));
            }
            context->next_param = NONE;
            break;
        case NONE:
            break;
        default:
            PRINTF("Param not supported: %d\n", context->next_param);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            break;
    }
}

static void handle_operator_reward(ethPluginProvideParameter_t *msg, context_t *context) {
    switch (context->next_param) {
        case OPERATOR:
            copy_address(context->tx.body.update_operator_reward.operator.value,
                         msg->parameter,
                         sizeof(context->tx.body.update_operator_reward.operator.value));
            context->next_param = REWARD;
            break;
        case REWARD:
            copy_address(context->tx.body.update_operator_reward.reward.value,
                         msg->parameter,
                         sizeof(context->tx.body.update_operator_reward.reward.value));
            context->next_param = NONE;
            break;
        case NONE:
            break;
        default:
            PRINTF("Param not supported: %d\n", context->next_param);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            break;
    }
}

static void handle_operator_address(ethPluginProvideParameter_t *msg, context_t *context) {
    switch (context->next_param) {
        case OPERATOR:
            copy_address(context->tx.body.update_operator_address.operator.value,
                         msg->parameter,
                         sizeof(context->tx.body.update_operator_address.operator.value));
            context->next_param = ADDRESS;
            break;
        case ADDRESS:
            copy_address(context->tx.body.update_operator_address.new_operator.value,
                         msg->parameter,
                         sizeof(context->tx.body.update_operator_address.new_operator.value));
            context->next_param = NONE;
            break;
        case NONE:
            break;
        default:
            PRINTF("Param not supported: %d\n", context->next_param);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            break;
    }
}

static void handle_operator_initialize(ethPluginProvideParameter_t *msg, context_t *context) {
    switch (context->next_param) {
        case ADDRESS:
            copy_address(context->tx.body.initialize.control.value,
                         msg->parameter,
                         sizeof(context->tx.body.initialize.control.value));
            context->next_param = NONE;
            break;
        case NONE:
            break;
        default:
            PRINTF("Param not supported: %d\n", context->next_param);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            break;
    }
}

static void handle_operator_disable_enable(ethPluginProvideParameter_t *msg, context_t *context) {
    switch (context->next_param) {
        case ADDRESS:
            copy_address(context->tx.body.disable_enable_operator.operator.value,
                         msg->parameter,
                         sizeof(context->tx.body.disable_enable_operator.operator.value));
            context->next_param = NONE;
            break;
        case NONE:
            break;
        default:
            PRINTF("Param not supported: %d\n", context->next_param);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            break;
    }
}

static void handle_add_operator(ethPluginProvideParameter_t *msg, context_t *context) {
    uint16_t containers = 0;  // group of 32 bytes needed to hold name

    if (context->go_to_offset) {
        if (msg->parameterOffset != context->offset) {
            return;
        }
        context->go_to_offset = false;
    }

    switch (context->next_param) {
        case NAME_OFFSET:
            // save temporarily the offset to check on the name len parameter we are indeed in the
            // right offset
            if (!U2BE_from_parameter(msg->parameter, &context->tx.body.add_operator.name_offset)) {
                msg->result = ETH_PLUGIN_RESULT_ERROR;
                return;
            }
            context->next_param = OPERATOR;
            break;
        case OPERATOR:
            copy_address(context->tx.body.add_operator.operator.value,
                         msg->parameter,
                         sizeof(context->tx.body.add_operator.operator.value));
            context->next_param = REWARD;
            break;
        case REWARD:
            copy_address(context->tx.body.add_operator.reward.value,
                         msg->parameter,
                         sizeof(context->tx.body.add_operator.reward.value));
            context->next_param = NAME_LEN;
            break;
        case NAME_LEN:
            if (msg->parameterOffset != context->tx.body.add_operator.name_offset + SELECTOR_SIZE) {
                msg->result = ETH_PLUGIN_RESULT_ERROR;
                return;
            }

            if (!U2BE_from_parameter(msg->parameter, &context->tx.body.add_operator.name.len)) {
                msg->result = ETH_PLUGIN_RESULT_ERROR;
                return;
            }
            context->next_param = NAME;
            break;
        case NAME:
            // Name has less then 32
            if (context->tx.body.add_operator.name.len <= PARAMETER_LENGTH) {
                copy_text(context->tx.body.add_operator.name.text,
                          context->tx.body.add_operator.name.len,
                          PARAMETER_LENGTH,
                          msg->parameter);
                context->tx.body.add_operator.name.ellipsis = false;
                context->next_param = NONE;
            } else {  // Name has more then 32 bytes
                context->tx.body.add_operator.name.ellipsis = true;
                containers = context->tx.body.add_operator.name.len / PARAMETER_LENGTH;
                bytes_missing = context->tx.body.add_operator.name.len % PARAMETER_LENGTH;

                // copy first 16 bytes
                copy_text(context->tx.body.add_operator.name.text,
                          context->tx.body.add_operator.name.len,
                          HALF_PARAMETER_LENGTH,
                          msg->parameter);

                if (bytes_missing < HALF_PARAMETER_LENGTH) {
                    if (containers < 2) {  // only one container we still need bytes from this one
                                           // to complete the last 16. then go to last container
                        copy_text(context->tx.body.add_operator.name.text + HALF_PARAMETER_LENGTH,
                                  context->tx.body.add_operator.name.len - HALF_PARAMETER_LENGTH,
                                  HALF_PARAMETER_LENGTH - bytes_missing,
                                  msg->parameter + HALF_PARAMETER_LENGTH + bytes_missing);

                        context->offset = msg->parameterOffset + PARAMETER_LENGTH;
                        context->go_to_offset = true;
                        context->next_param = NAME_OFFSET_2;
                    } else {  // more then 1 container go to second-last and get missing bytes to
                              // complete the last 16
                        context->offset =
                            msg->parameterOffset + (containers - 1) * PARAMETER_LENGTH;
                        context->go_to_offset = true;
                        context->next_param = NAME_OFFSET_1;
                    }
                } else {  // last container has the last 16 bytes we need
                    context->offset = msg->parameterOffset + containers * PARAMETER_LENGTH;
                    context->go_to_offset = true;
                    context->next_param = NAME_OFFSET_2;
                }
            }
            break;
        case NAME_OFFSET_1:  // second last container
            copy_text(context->tx.body.add_operator.name.text + HALF_PARAMETER_LENGTH,
                      context->tx.body.add_operator.name.len - HALF_PARAMETER_LENGTH,
                      HALF_PARAMETER_LENGTH - bytes_missing,
                      msg->parameter + HALF_PARAMETER_LENGTH + bytes_missing);
            context->next_param = NAME_OFFSET_2;
            break;
        case NAME_OFFSET_2:                                // last container
            if (bytes_missing <= HALF_PARAMETER_LENGTH) {  // copy missing bytes
                copy_text(context->tx.body.add_operator.name.text + HALF_PARAMETER_LENGTH +
                              (HALF_PARAMETER_LENGTH - bytes_missing),
                          context->tx.body.add_operator.name.len - HALF_PARAMETER_LENGTH +
                              (HALF_PARAMETER_LENGTH - bytes_missing),
                          bytes_missing,
                          msg->parameter);
            } else {  // last container has 16 or more bytes, move the need offset to copy the last
                      // 16 bytes
                copy_text(context->tx.body.add_operator.name.text + HALF_PARAMETER_LENGTH,
                          context->tx.body.add_operator.name.len - HALF_PARAMETER_LENGTH,
                          HALF_PARAMETER_LENGTH,
                          msg->parameter + (bytes_missing - HALF_PARAMETER_LENGTH));
            }
            context->next_param = NONE;
            break;
        case NONE:
            break;
        default:
            PRINTF("Param not supported: %d\n", context->next_param);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            break;
    }
}

static void handle_pubkeys(ethPluginProvideParameter_t *msg, context_t *context) {
    uint16_t containers = 0;  // group of 32 bytes needed to hold name
    uint16_t tmp_offset = 0;

    if (context->go_to_offset) {
        if (msg->parameterOffset != context->offset) {
            return;
        }
        context->go_to_offset = false;
    }
    switch (context->next_param) {
        case OFFSET:
            // save temporarily the offset to check on the next parameter we are indeed in the right
            // offset
            if (!U2BE_from_parameter(msg->parameter,
                                     &context->tx.body.pubkey_methods.offsets_start)) {
                msg->result = ETH_PLUGIN_RESULT_ERROR;
                return;
            }
            context->next_param = N_PUBKEYS;
            break;
        case N_PUBKEYS:
            if (msg->parameterOffset !=
                context->tx.body.pubkey_methods.offsets_start + SELECTOR_SIZE) {
                msg->result = ETH_PLUGIN_RESULT_ERROR;
                return;
            }

            if (!U2BE_from_parameter(msg->parameter, &context->tx.body.pubkey_methods.n_pubkeys) ||
                context->tx.body.pubkey_methods.n_pubkeys > 4 ||
                context->tx.body.pubkey_methods.n_pubkeys == 0) {
                msg->result = ETH_PLUGIN_RESULT_ERROR;
            }
            context->next_param = OFFSETS;
            break;
        case OFFSETS:
            if (!U2BE_from_parameter(msg->parameter, &tmp_offset)) {
                msg->result = ETH_PLUGIN_RESULT_ERROR;
                return;
            }

            // save offsets start point
            if (context->tx.body.pubkey_methods.id == 0) {
                context->tx.body.pubkey_methods.offsets_start = msg->parameterOffset;
            }
            // save offset and if not last one save next
            context->tx.body.pubkey_methods.offsets[context->tx.body.pubkey_methods.id] =
                tmp_offset;
            context->tx.body.pubkey_methods.id++;
            context->next_param = OFFSETS;
            if (context->tx.body.pubkey_methods.id == context->tx.body.pubkey_methods.n_pubkeys) {
                context->tx.body.pubkey_methods.id = 0;
                context->offset = context->tx.body.pubkey_methods.offsets_start +
                                  context->tx.body.pubkey_methods.offsets[0];
                context->go_to_offset = true;
                context->next_param = PUBKEY_LEN;
            }
            break;
        case PUBKEY_LEN:
            if (!U2BE_from_parameter(msg->parameter,
                                     &context->tx.body.pubkey_methods
                                          .pubkeys_len[context->tx.body.pubkey_methods.id])) {
                msg->result = ETH_PLUGIN_RESULT_ERROR;
                return;
            }
            context->next_param = PUBKEY;
            break;
        case PUBKEY:
            // Name has less then 32
            if (context->tx.body.pubkey_methods.pubkeys_len[context->tx.body.pubkey_methods.id] <=
                PARAMETER_LENGTH) {
                copy_text(context->tx.body.pubkey_methods.pubkey[context->tx.body.pubkey_methods.id]
                              .value,
                          sizeof(context->tx.body.pubkey_methods
                                     .pubkey[context->tx.body.pubkey_methods.id]
                                     .value),
                          PARAMETER_LENGTH,
                          msg->parameter);
                context->tx.body.pubkey_methods.pubkey[context->tx.body.pubkey_methods.id]
                    .ellipsis = false;
                if (context->tx.body.pubkey_methods.id ==
                    context->tx.body.pubkey_methods.n_pubkeys - 1) {
                    context->tx.body.pubkey_methods.id = 0;
                    context->next_param = NONE;
                } else {
                    context->tx.body.pubkey_methods.id++;
                    context->offset =
                        context->tx.body.pubkey_methods.offsets_start +
                        context->tx.body.pubkey_methods.offsets[context->tx.body.pubkey_methods.id];
                    context->go_to_offset = true;
                    context->next_param = PUBKEY_LEN;
                }
            } else {  // Name has more then 32 bytes
                containers = context->tx.body.pubkey_methods
                                 .pubkeys_len[context->tx.body.pubkey_methods.id] /
                             PARAMETER_LENGTH;
                bytes_missing = context->tx.body.pubkey_methods
                                    .pubkeys_len[context->tx.body.pubkey_methods.id] %
                                PARAMETER_LENGTH;
                context->tx.body.pubkey_methods.pubkey[context->tx.body.pubkey_methods.id]
                    .ellipsis = true;
                // copy first 16 bytes
                copy_text(context->tx.body.pubkey_methods.pubkey[context->tx.body.pubkey_methods.id]
                              .value,
                          sizeof(context->tx.body.pubkey_methods
                                     .pubkey[context->tx.body.pubkey_methods.id]
                                     .value),
                          HALF_PARAMETER_LENGTH,
                          msg->parameter);

                if (bytes_missing < HALF_PARAMETER_LENGTH) {
                    if (containers < 2) {  // only one container we still need bytes from this one
                                           // to complete the last 16. then go to last container
                        copy_text(context->tx.body.pubkey_methods
                                          .pubkey[context->tx.body.pubkey_methods.id]
                                          .value +
                                      HALF_PARAMETER_LENGTH,
                                  sizeof(context->tx.body.pubkey_methods
                                             .pubkey[context->tx.body.pubkey_methods.id]
                                             .value) -
                                      HALF_PARAMETER_LENGTH,
                                  HALF_PARAMETER_LENGTH - bytes_missing,
                                  msg->parameter + HALF_PARAMETER_LENGTH + bytes_missing);

                        context->offset = msg->parameterOffset + PARAMETER_LENGTH;
                        context->go_to_offset = true;
                        context->next_param = PUBKEY_OFFSET_2;
                    } else {  // more then 1 container go to second-last and get missing bytes to
                              // complete the last 16
                        context->offset =
                            msg->parameterOffset + (containers - 1) * PARAMETER_LENGTH;
                        context->go_to_offset = true;
                        context->next_param = PUBKEY_OFFSET_1;
                    }
                } else {  // last container has the last 16 bytes we need
                    context->offset = msg->parameterOffset + containers * PARAMETER_LENGTH;
                    context->go_to_offset = true;
                    context->next_param = PUBKEY_OFFSET_2;
                }
            }
            break;
        case PUBKEY_OFFSET_1:  // second last container
            copy_text(
                context->tx.body.pubkey_methods.pubkey[context->tx.body.pubkey_methods.id].value +
                    HALF_PARAMETER_LENGTH,
                sizeof(context->tx.body.pubkey_methods.pubkey[context->tx.body.pubkey_methods.id]
                           .value) -
                    HALF_PARAMETER_LENGTH,
                HALF_PARAMETER_LENGTH - bytes_missing,
                msg->parameter + HALF_PARAMETER_LENGTH + bytes_missing);
            context->next_param = PUBKEY_OFFSET_2;
            break;
        case PUBKEY_OFFSET_2:                              // last container
            if (bytes_missing <= HALF_PARAMETER_LENGTH) {  // copy missing bytes
                copy_text(context->tx.body.pubkey_methods.pubkey[context->tx.body.pubkey_methods.id]
                                  .value +
                              HALF_PARAMETER_LENGTH + (HALF_PARAMETER_LENGTH - bytes_missing),
                          sizeof(context->tx.body.pubkey_methods
                                     .pubkey[context->tx.body.pubkey_methods.id]
                                     .value) -
                              HALF_PARAMETER_LENGTH + (HALF_PARAMETER_LENGTH - bytes_missing),
                          bytes_missing,
                          msg->parameter);
            } else {  // last container has 16 or more bytes, move the need offset to copy the last
                      // 16 bytes
                copy_text(context->tx.body.pubkey_methods.pubkey[context->tx.body.pubkey_methods.id]
                                  .value +
                              HALF_PARAMETER_LENGTH,
                          sizeof(context->tx.body.pubkey_methods
                                     .pubkey[context->tx.body.pubkey_methods.id]
                                     .value) -
                              HALF_PARAMETER_LENGTH,
                          HALF_PARAMETER_LENGTH,
                          msg->parameter + (bytes_missing - HALF_PARAMETER_LENGTH));
            }

            if (context->tx.body.pubkey_methods.id ==
                context->tx.body.pubkey_methods.n_pubkeys - 1) {
                context->tx.body.pubkey_methods.id = 0;
                context->next_param = NONE;
            } else {
                // we are not on the last offset, jump to next pubkey
                context->tx.body.pubkey_methods.id++;
                context->offset =
                    context->tx.body.pubkey_methods.offsets_start +
                    context->tx.body.pubkey_methods.offsets[context->tx.body.pubkey_methods.id];
                context->go_to_offset = true;
                context->next_param = PUBKEY_LEN;
            }
            break;
        case NONE:
            break;
        default:
            PRINTF("Param not supported: %d\n", context->next_param);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            break;
    }
}

static void handle_withdraw(ethPluginProvideParameter_t *msg, context_t *context) {
    switch (context->next_param) {
        case ADDRESS:
            copy_address(context->tx.body.withdrawerc20.token_addr.value,
                         msg->parameter,
                         sizeof(context->tx.body.withdrawerc20.token_addr.value));
            context->next_param = NONE;
            break;
        case NONE:
            break;
        default:
            PRINTF("Param not supported: %d\n", context->next_param);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            break;
    }
}

static void handle_add_new_validator(ethPluginProvideParameter_t *msg, context_t *context) {
    uint16_t containers = 0;  // group of 32 bytes needed to hold name
    uint16_t tmp_offset = 0;

    if (context->go_to_offset) {
        if (msg->parameterOffset != context->offset) {
            return;
        }
        context->go_to_offset = false;
    }
    switch (context->next_param) {
        case OFFSET:
            // save temporarily the offset to check on the next parameter we are indeed in the right
            // offset
            if (!U2BE_from_parameter(msg->parameter,
                                     &context->tx.body.add_new_validator.tuple_offsets_start)) {
                msg->result = ETH_PLUGIN_RESULT_ERROR;
                return;
            }
            context->next_param = N_PUBKEYS;
            break;
        case N_PUBKEYS:
            if (msg->parameterOffset !=
                context->tx.body.add_new_validator.tuple_offsets_start + SELECTOR_SIZE) {
                msg->result = ETH_PLUGIN_RESULT_ERROR;
                return;
            }

            if (!U2BE_from_parameter(msg->parameter,
                                     &context->tx.body.add_new_validator.n_pubkeys) ||
                context->tx.body.add_new_validator.n_pubkeys > 4 ||
                context->tx.body.add_new_validator.n_pubkeys == 0) {
                msg->result = ETH_PLUGIN_RESULT_ERROR;
            }
            context->next_param = OFFSETS;
            break;
        case OFFSETS:
            if (!U2BE_from_parameter(msg->parameter, &tmp_offset)) {
                msg->result = ETH_PLUGIN_RESULT_ERROR;
                return;
            }

            // save offsets start point
            if (context->tx.body.add_new_validator.id == 0) {
                context->tx.body.add_new_validator.tuple_offsets_start = msg->parameterOffset;
            }
            // save offset and if not last one save next
            context->tx.body.add_new_validator
                .tuple_offsets[context->tx.body.add_new_validator.id] = tmp_offset;
            context->tx.body.add_new_validator.id++;
            context->next_param = OFFSETS;
            if (context->tx.body.add_new_validator.id ==
                context->tx.body.add_new_validator.n_pubkeys) {
                context->tx.body.add_new_validator.id = 0;
                context->offset = context->tx.body.add_new_validator.tuple_offsets_start +
                                  context->tx.body.add_new_validator.tuple_offsets[0];
                context->go_to_offset = true;
                context->next_param = TUPLE_OFFSET_1;
            }
            break;
        case TUPLE_OFFSET_1:
            if (!U2BE_from_parameter(msg->parameter, &tmp_offset)) {
                msg->result = ETH_PLUGIN_RESULT_ERROR;
                return;
            }
            context->offset = msg->parameterOffset + tmp_offset;
            context->go_to_offset = true;
            context->next_param = PUBKEY_LEN;
            break;
        case PUBKEY_LEN:
            if (!U2BE_from_parameter(msg->parameter,
                                     &context->tx.body.add_new_validator
                                          .pubkeys_len[context->tx.body.add_new_validator.id])) {
                msg->result = ETH_PLUGIN_RESULT_ERROR;
                return;
            }
            context->next_param = PUBKEY;
            break;
        case PUBKEY:
            // Name has less then 32
            if (context->tx.body.add_new_validator
                    .pubkeys_len[context->tx.body.add_new_validator.id] <= PARAMETER_LENGTH) {
                copy_text(
                    context->tx.body.add_new_validator.pubkey[context->tx.body.add_new_validator.id]
                        .value,
                    sizeof(context->tx.body.add_new_validator
                               .pubkey[context->tx.body.add_new_validator.id]
                               .value),
                    PARAMETER_LENGTH,
                    msg->parameter);
                context->tx.body.add_new_validator.pubkey[context->tx.body.add_new_validator.id]
                    .ellipsis = false;
                if (context->tx.body.add_new_validator.id ==
                    context->tx.body.add_new_validator.n_pubkeys - 1) {
                    context->tx.body.add_new_validator.id = 0;
                    context->next_param = NONE;
                } else {
                    context->tx.body.add_new_validator.id++;
                    context->offset = context->tx.body.add_new_validator.tuple_offsets_start +
                                      context->tx.body.add_new_validator
                                          .tuple_offsets[context->tx.body.add_new_validator.id];
                    context->go_to_offset = true;
                    context->next_param = TUPLE_OFFSET_1;
                }
            } else {  // Name has more then 32 bytes
                containers = context->tx.body.add_new_validator
                                 .pubkeys_len[context->tx.body.add_new_validator.id] /
                             PARAMETER_LENGTH;
                bytes_missing = context->tx.body.add_new_validator
                                    .pubkeys_len[context->tx.body.add_new_validator.id] %
                                PARAMETER_LENGTH;
                context->tx.body.add_new_validator.pubkey[context->tx.body.add_new_validator.id]
                    .ellipsis = true;
                // copy first 16 bytes
                copy_text(
                    context->tx.body.add_new_validator.pubkey[context->tx.body.add_new_validator.id]
                        .value,
                    sizeof(context->tx.body.add_new_validator
                               .pubkey[context->tx.body.add_new_validator.id]
                               .value),
                    HALF_PARAMETER_LENGTH,
                    msg->parameter);

                if (bytes_missing < HALF_PARAMETER_LENGTH) {
                    if (containers < 2) {  // only one container we still need bytes from this one
                                           // to complete the last 16. then go to last container
                        copy_text(context->tx.body.pubkey_methods
                                          .pubkey[context->tx.body.pubkey_methods.id]
                                          .value +
                                      HALF_PARAMETER_LENGTH,
                                  sizeof(context->tx.body.pubkey_methods
                                             .pubkey[context->tx.body.pubkey_methods.id]
                                             .value) -
                                      HALF_PARAMETER_LENGTH,
                                  HALF_PARAMETER_LENGTH - bytes_missing,
                                  msg->parameter + HALF_PARAMETER_LENGTH + bytes_missing);

                        context->offset = msg->parameterOffset + PARAMETER_LENGTH;
                        context->go_to_offset = true;
                        context->next_param = PUBKEY_OFFSET_2;
                    } else {  // more then 1 container go to second-last and get missing bytes to
                              // complete the last 16
                        context->offset =
                            msg->parameterOffset + (containers - 1) * PARAMETER_LENGTH;
                        context->go_to_offset = true;
                        context->next_param = PUBKEY_OFFSET_1;
                    }
                } else {  // last container has the last 16 bytes we need
                    context->offset = msg->parameterOffset + containers * PARAMETER_LENGTH;
                    context->go_to_offset = true;
                    context->next_param = PUBKEY_OFFSET_2;
                }
            }
            break;
        case PUBKEY_OFFSET_1:  // second last container
            copy_text(
                context->tx.body.add_new_validator.pubkey[context->tx.body.add_new_validator.id]
                        .value +
                    HALF_PARAMETER_LENGTH,
                sizeof(
                    context->tx.body.add_new_validator.pubkey[context->tx.body.add_new_validator.id]
                        .value) -
                    HALF_PARAMETER_LENGTH,
                HALF_PARAMETER_LENGTH - bytes_missing,
                msg->parameter + HALF_PARAMETER_LENGTH + bytes_missing);
            context->next_param = PUBKEY_OFFSET_2;
            break;
        case PUBKEY_OFFSET_2:                              // last container
            if (bytes_missing <= HALF_PARAMETER_LENGTH) {  // copy missing bytes
                copy_text(
                    context->tx.body.add_new_validator.pubkey[context->tx.body.add_new_validator.id]
                            .value +
                        HALF_PARAMETER_LENGTH + (HALF_PARAMETER_LENGTH - bytes_missing),
                    sizeof(context->tx.body.add_new_validator
                               .pubkey[context->tx.body.add_new_validator.id]
                               .value) -
                        HALF_PARAMETER_LENGTH + (HALF_PARAMETER_LENGTH - bytes_missing),
                    bytes_missing,
                    msg->parameter);
            } else {  // last container has 16 or more bytes, move the need offset to copy the last
                      // 16 bytes
                copy_text(
                    context->tx.body.add_new_validator.pubkey[context->tx.body.add_new_validator.id]
                            .value +
                        HALF_PARAMETER_LENGTH,
                    sizeof(context->tx.body.add_new_validator
                               .pubkey[context->tx.body.add_new_validator.id]
                               .value) -
                        HALF_PARAMETER_LENGTH,
                    HALF_PARAMETER_LENGTH,
                    msg->parameter + (bytes_missing - HALF_PARAMETER_LENGTH));
            }
            if (context->tx.body.add_new_validator.id ==
                context->tx.body.add_new_validator.n_pubkeys - 1) {
                context->tx.body.add_new_validator.id = 0;
                context->next_param = NONE;
            } else {
                // we are not on the last offset, jump to next tuple
                context->tx.body.add_new_validator.id++;
                context->offset = context->tx.body.add_new_validator.tuple_offsets_start +
                                  context->tx.body.add_new_validator
                                      .tuple_offsets[context->tx.body.add_new_validator.id];
                context->go_to_offset = true;
                context->next_param = TUPLE_OFFSET_1;
            }
            break;
        case NONE:
            break;
        default:
            PRINTF("Param not supported: %d\n", context->next_param);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            break;
    }
}

void handle_provide_parameter(ethPluginProvideParameter_t *msg) {
    context_t *context = (context_t *) msg->pluginContext;
    // We use `%.*H`: it's a utility function to print bytes. You first give
    // the number of bytes you wish to print (in this case, `PARAMETER_LENGTH`) and then
    // the address (here `msg->parameter`).
    PRINTF("plugin provide parameter: offset %d\nBytes: %.*H\n",
           msg->parameterOffset,
           PARAMETER_LENGTH,
           msg->parameter);

    msg->result = ETH_PLUGIN_RESULT_OK;

    switch (context->selectorIndex) {
        case ADD_NEW_VALIDATOR:
            handle_add_new_validator(msg, context);
            break;
        case ADD_OPERATOR:
            handle_add_operator(msg, context);
            break;
        case DELETE_ACTIVE_VALIDATORS:
        case DELETE_PENDING_VALIDATORS:
        case USE_PUBKEYS_FOR_VALIDATOR:
            handle_pubkeys(msg, context);
            break;
        case DISABLE_OPERATOR:
        case ENABLE_OPERATOR:
            handle_operator_disable_enable(msg, context);
            break;
        case INITIALIZE:
            handle_operator_initialize(msg, context);
            break;
        case UPDATE_OPERATOR_ADDRESS:
            handle_operator_address(msg, context);
            break;
        case UPDATE_OPERATOR_NAME:
            handle_operator_name(msg, context);
            break;
        case UPDATE_OPERATOR_REWARD:
            handle_operator_reward(msg, context);
            break;
        case WITHDRAWERC20:
            handle_withdraw(msg, context);
            break;
        default:
            PRINTF("Selector Index not supported: %d\n", context->selectorIndex);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            break;
    }
}
