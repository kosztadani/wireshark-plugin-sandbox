#include <ws_version.h>
#if WIRESHARK_VERSION_MAJOR <= 2 || (WIRESHARK_VERSION_MAJOR == 3 && WIRESHARK_VERSION_MINOR < 6)
#warning "Not tested with Wireshark version <3.6"
#elif (WIRESHARK_VERSION_MAJOR == 4 && WIRESHARK_VERSION_MINOR > 4) || WIRESHARK_VERSION_MAJOR > 4
#warning "Not tested with Wireshark version >4.4"
#endif

#define WS_BUILD_DLL
#include <epan/packet.h>
#include <epan/proto.h>
#include <epan/address.h>
#include <epan/dissectors/packet-tcp.h>

#if WIRESHARK_VERSION_MAJOR > 4
#include <wireshark.h>
#else
#include <stdint.h>
#include <ws_symbol_export.h>
#endif

#if (WIRESHARK_VERSION_MAJOR == 4 && WIRESHARK_VERSION_MINOR >= 4) || WIRESHARK_VERSION_MAJOR >= 5
#include <wsutil/plugins.h>
#else
#define WS_PLUGIN_DESC_DISSECTOR    (1UL << 0)
#endif

#include "my-math.h"

#ifndef VERSION
#define VERSION "0.0.0"
#endif

WS_DLL_PUBLIC_DEF const char plugin_version[] = VERSION;
WS_DLL_PUBLIC_DEF const int plugin_want_major = WIRESHARK_VERSION_MAJOR;
WS_DLL_PUBLIC_DEF const int plugin_want_minor = WIRESHARK_VERSION_MINOR;

static int proto_my_math;
static dissector_handle_t handle_my_math;

// enums
static const value_string optypes[] = {
        {MY_MATH_OPTYPE_ADD, "Add"},
        {MY_MATH_OPTYPE_SUB, "Subtract"},
        {MY_MATH_OPTYPE_DIV, "Divide"},
        {MY_MATH_OPTYPE_MUL, "Multiply"}
};

// subtrees
#if (WIRESHARK_VERSION_MAJOR == 4 && WIRESHARK_VERSION_MINOR >= 4) || WIRESHARK_VERSION_MAJOR >= 5
static int ett_my_math;
#else
static gint ett_my_math = -1;
#endif

static void proto_register_my_math(void);

static void proto_register_handoff_my_math(void);

static int dissect_my_math(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, void *data);

static unsigned get_my_math_message_len(packet_info *pinfo, tvbuff_t *tvb, int offset, void *data);

static int dissect_my_math_message(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, void *data);

static bool is_request(packet_info *pinfo);

static char operation_sign(uint32_t opcode);


// fields
static int hf_optype;
static int hf_first_operand;
static int hf_second_operand;
static int hf_result;

void plugin_register(void) {
        static proto_plugin plug;
        plug.register_protoinfo = proto_register_my_math;
        plug.register_handoff = proto_register_handoff_my_math;
        proto_register_plugin(&plug);
}

uint32_t plugin_describe(void) {
        return WS_PLUGIN_DESC_DISSECTOR;
}

static void proto_register_my_math(void) {
        static hf_register_info hf[] = {
                {
                        &hf_optype,
                        {"Operation type", "my-math.optype", FT_UINT8, BASE_DEC, VALS(optypes), 0x0,NULL, HFILL}
                },
                {
                        &hf_first_operand,
                        {"First operand", "my-math.first-operand", FT_INT32, BASE_DEC, NULL, 0x0, NULL, HFILL}
                },
                {
                        &hf_second_operand,
                        {"Second operand", "my-math.second-operand", FT_INT32, BASE_DEC, NULL, 0x0, NULL, HFILL}
                },
                {
                        &hf_result,
                        {"Result", "my-math.result", FT_INT32, BASE_DEC, NULL, 0x0, NULL, HFILL}
                },
        };
#if (WIRESHARK_VERSION_MAJOR == 4 && WIRESHARK_VERSION_MINOR >= 4) || WIRESHARK_VERSION_MAJOR >= 5
        static int *ett[] = {
#else
        static gint *ett[] = {
#endif
                &ett_my_math
        };
        proto_my_math = proto_register_protocol("My Simple Math Protocol", "MY-MATH", "my-math");
        proto_register_field_array(proto_my_math, hf, array_length(hf));
        proto_register_subtree_array(ett, array_length(ett));
}

static void proto_register_handoff_my_math(void) {
        handle_my_math = create_dissector_handle(dissect_my_math, proto_my_math);
        dissector_add_for_decode_as("tcp.port", handle_my_math);
}

static int dissect_my_math(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, void *data) {
        col_set_str(pinfo->cinfo, COL_PROTOCOL, "MY-MATH");
        tcp_dissect_pdus(tvb, pinfo, tree, true, 0, get_my_math_message_len, dissect_my_math_message, data);
        return tvb_reported_length(tvb);
}

static unsigned get_my_math_message_len(packet_info *pinfo, tvbuff_t *tvb _U_, int offset _U_, void *data _U_) {
        return is_request(pinfo) ? MY_MATH_REQUEST_SIZE : MY_MATH_RESPONSE_SIZE;
}

static int dissect_my_math_message(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, void *data _U_) {
        if (is_request(pinfo)) {
                proto_item *ti = proto_tree_add_item(tree, proto_my_math, tvb, 0, MY_MATH_REQUEST_SIZE, ENC_NA);
                proto_tree *my_math_tree = proto_item_add_subtree(ti, ett_my_math);
                uint32_t opcode;
                proto_tree_add_item_ret_uint(my_math_tree, hf_optype, tvb, 0, 1, ENC_BIG_ENDIAN, &opcode);
                int32_t first;
                proto_tree_add_item_ret_int(my_math_tree, hf_first_operand, tvb, 1, 4, ENC_BIG_ENDIAN, &first);
                int32_t second;
                proto_tree_add_item_ret_int(my_math_tree, hf_second_operand, tvb, 5, 4, ENC_BIG_ENDIAN, &second);
                col_add_fstr(pinfo->cinfo, COL_INFO, "My Simple Math Protocol: Request (%i %c %i)",
                             first, operation_sign(opcode), second);
                return MY_MATH_REQUEST_SIZE;
        } else {
                proto_item *ti = proto_tree_add_item(tree, proto_my_math, tvb, 0, MY_MATH_RESPONSE_SIZE, ENC_NA);
                proto_tree *my_math_tree = proto_item_add_subtree(ti, ett_my_math);
                int32_t result;
                proto_tree_add_item_ret_int(my_math_tree, hf_result, tvb, 0, 4, ENC_BIG_ENDIAN, &result);
                col_add_fstr(pinfo->cinfo, COL_INFO, "My Simple Math Protocol: Response (%i)", result);
                return MY_MATH_RESPONSE_SIZE;
        }
}

static bool is_request(packet_info *pinfo) {
        conversation_t *conversation = find_or_create_conversation(pinfo);
        struct my_math_conversation_data *data = conversation_get_proto_data(conversation, proto_my_math);
        if (data == NULL) {
                wmem_allocator_t *scope = wmem_file_scope();
                data = wmem_alloc(scope, sizeof(struct my_math_conversation_data));
                copy_address_wmem(scope, &data->server_address, &pinfo->dst);
                data->server_port = pinfo->destport;
                conversation_add_proto_data(conversation, proto_my_math, data);
                return true;
        } else {
                return cmp_address(&pinfo->dst, &data->server_address) == 0 &&
                       pinfo->destport == data->server_port;
        }
}

static char operation_sign(const uint32_t opcode) {
        switch (opcode) {
                case MY_MATH_OPTYPE_ADD:
                        return '+';
                case MY_MATH_OPTYPE_DIV:
                        return '/';
                case MY_MATH_OPTYPE_SUB:
                        return '-';
                case MY_MATH_OPTYPE_MUL:
                        return '*';
                default:
                        return ' ';
        }
}
