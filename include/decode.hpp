#pragma once
#include <assert.h>
#include <cstdint>
#include <iostream>
#include <span>
#include <string>
#include <unordered_map>
#include <vector>

namespace Base64 {
constexpr static char base64_chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                       "abcdefghijklmnopqrstuvwxyz"
                                       "0123456789+/";
inline static const std::unordered_map<char, uint8_t> base64_map = [] {
    std::unordered_map<char, uint8_t> m;
    for (uint8_t i = 0; i < 64; ++i) {
        m[base64_chars[i]] = i;
    }
    return m;
}();

static std::vector<uint8_t> encode(const std::string_view s) {
    // 每三个8bit字节转换为四个6bit字节
    unsigned char array3[3];
    unsigned char array4[4];
    unsigned group = s.size() / 3;
    unsigned remain = s.size() % 3;
    int pos = 0;
    std::vector<uint8_t> ret(group * 4 + (remain == 0 ? 0 : 4));
    for (unsigned i = 0; i < group; ++i) {
        for (int j = 0; j < 3; ++j) {
            array3[j] = s[pos++];
        }
        array4[0] = (array3[0] & 0xfc) >> 2;
        array4[1] = ((array3[0] & 0x03) << 4) + ((array3[1] & 0xf0) >> 4);
        array4[2] = ((array3[1] & 0x0f) << 2) + ((array3[2] & 0xc0) >> 6);
        array4[3] = array3[2] & 0x3f;
        for (int j = 0; j < 4; ++j) {
            ret.push_back(base64_chars[array4[j]]);
        }
    }
    if (remain != 0) {
        for (int i = 0; i < remain; ++i) {
            array3[i] = s[pos++];
        }
        for (int i = remain; i < 4; ++i) {
            array3[i] = 0;
        }
        array4[0] = (array3[0] & 0xFC) >> 2;
        array4[1] = ((array3[0] & 0x03) << 4) + ((array3[1] & 0xF0) >> 4);
        array4[2] = ((array3[1] & 0x0F) << 2) + ((array3[2] & 0xC0) >> 6);
        array4[3] = array3[2] & 0x3F;
        for (int i = 0; i < remain + 1; ++i) {
            ret.push_back(base64_chars[array4[i]]);
        }
        for (int i = remain + 1; i < 4; ++i) {
            ret.push_back('=');
        }
    }
    return ret;
}

static std::vector<uint8_t> decode(const std::string_view s) {
    unsigned char array4[4], array3[3];
    std::vector<uint8_t> ret;
    int pos = 0;

    auto span_data = s.data();
    for (size_t i = 0; i < s.size(); ++i) {
        if (span_data[i] == '=') {
            break;
        }
        if (base64_map.find(static_cast<char>(span_data[i])) ==
            base64_map.end()) {
            continue; // Skip invalid characters
        }
        // 不存在会抛出异常
        array4[pos++] = base64_map.at(span_data[i]);

        if (pos == 4) {
            array3[0] = (array4[0] << 2) + ((array4[1] & 0x30) >> 4);
            array3[1] = ((array4[1] & 0xf) << 4) + ((array4[2] & 0x3c) >> 2);
            array3[2] = ((array4[2] & 0x3) << 6) + array4[3];

            for (int j = 0; j < 3; ++j) {
                ret.push_back(array3[j]);
            }
            pos = 0;
        }
    }

    if (pos > 0) {
        for (int j = pos; j < 4; ++j) {
            array4[j] = 0;
        }
        array3[0] = (array4[0] << 2) + ((array4[1] & 0x30) >> 4);
        array3[1] = ((array4[1] & 0xf) << 4) + ((array4[2] & 0x3c) >> 2);
        array3[2] = ((array4[2] & 0x3) << 6) + array4[3];
        for (int j = 0; j < pos - 1; ++j) {
            ret.push_back(array3[j]);
        }
    }
    char buf[24];
    return ret;
}
}; // namespace Base64