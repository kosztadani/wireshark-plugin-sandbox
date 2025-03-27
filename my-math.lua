local module = {
    proto = Proto.new("MY-MATH", "My Simple Math Protocol"),
    request_size = 9,
    response_size = 4,
    minimum_size = 4,
    enums = {
        optype = {
            [1] = "Add",
            [2] = "Subtract",
            [3] = "Multiply",
            [4] = "Divide",
        },
        optype_sign = {
            [1] = "+",
            [2] = "-",
            [3] = "*",
            [4] = "/",
        },
    }
}

function module:initialize()
    self.fields = {
        ["optype"] = ProtoField.new("Operation type", "my-math.optype", ftypes.UINT8, self.enums.optype),
        ["first_operand"] = ProtoField.new("First operand", "my-math.first-operand", ftypes.INT32),
        ["second_operand"] = ProtoField.new("Second operand", "my-math.second-operand", ftypes.INT32),
        ["result"] = ProtoField.new("Result", "my-math.result", ftypes.INT32),
    }
    for _, v in pairs(self.fields) do
        table.insert(self.proto.fields, v)
    end
    local tcp_table = DissectorTable.get("tcp.port")
    tcp_table:add_for_decode_as(self.proto)
end

function module.proto.dissector(tvb, pinfo, tree)
    module:dissect(tvb, pinfo, tree)
end

function module:dissect(tvb, pinfo, tree)
    pinfo.columns.protocol = "MY-MATH"
    pinfo.columns.info:clear()
    dissect_tcp_pdus(
            tvb,
            tree,
            self.minimum_size,
            function(tvb, pinfo, offset)
                return self:get_length(pinfo)
            end,
            function(tvb, pinfo, tree)
                return self:dissect_single(tvb, pinfo, tree)
            end
    )
end

function module:get_length(pinfo)
    if self:is_request(pinfo) then
        return self.request_size
    else
        return self.response_size
    end
end

function module:dissect_single(tvb, pinfo, tree)
    pinfo.columns.info:clear()
    if tostring(pinfo.columns.info) ~= "" then
        pinfo.columns.info:append(", ")
    end
    local tree = tree:add(self.proto, tvb())
    local size
    if self:is_request(pinfo) then
        size = self:dissect_request(tvb, pinfo, tree)
    else
        size = self:dissect_response(tvb, pinfo, tree)
    end
    pinfo.columns.info:fence()
    return size
end

function module:dissect_request(tvb, pinfo, tree)
    local _, optype = tree:add_packet_field(self.fields.optype, tvb(0, 1), ENC_BIG_ENDIAN)
    local _, first_operand = tree:add_packet_field(self.fields.first_operand, tvb(1, 4), ENC_BIG_ENDIAN)
    local _, second_operand = tree:add_packet_field(self.fields.second_operand, tvb(5, 4), ENC_BIG_ENDIAN)
    local optype_sign = self.enums.optype_sign[optype]
    local text = string.format(
            "Request (%i %s %i)",
            first_operand, optype_sign, second_operand
    )
    pinfo.columns.info:append(text)
    tree:append_text(", " .. text)
    return self.request_size
end

function module:dissect_response(tvb, pinfo, tree)
    local _, result = tree:add_packet_field(self.fields.result, tvb(0, 4), ENC_BIG_ENDIAN)
    local text = string.format(
            "Response (%i)",
            result
    )
    pinfo.columns.info:append(text)
    tree:append_text(", " .. text)
    return self.response_size
end

function module:is_request(pinfo)
    local conversation = Conversation.find_from_pinfo(pinfo)
    local data = conversation[self.proto]
    if data then
        return pinfo.dst == data.server_address and pinfo.dst_port == data.server_port
    else
        data = {
            server_address = pinfo.dst,
            server_port = pinfo.dst_port,
        }
        conversation[self.proto] = data
        return true
    end
end

module:initialize()