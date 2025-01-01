#ifndef MY_MATH_H
#define MY_MATH_H

WS_DLL_PUBLIC void plugin_register(void);

WS_DLL_PUBLIC uint32_t plugin_describe(void);

#define MY_MATH_REQUEST_SIZE (9)
#define MY_MATH_RESPONSE_SIZE (4)

#define MY_MATH_OPTYPE_ADD (0x01)
#define MY_MATH_OPTYPE_SUB (0x02)
#define MY_MATH_OPTYPE_MUL (0x03)
#define MY_MATH_OPTYPE_DIV (0x04)

struct my_math_conversation_data {
        address server_address;
        uint32_t server_port;
};

static void proto_register_my_math(void);

static void proto_register_handoff_my_math(void);

static int dissect_my_math(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, void *data);

static unsigned get_my_math_message_len(packet_info *pinfo, tvbuff_t *tvb, int offset, void *data);

static int dissect_my_math_message(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, void *data);

static bool is_request(packet_info *pinfo);

static char operation_sign(uint32_t opcode);

#endif //MY_MATH_H
