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

#endif //MY_MATH_H
