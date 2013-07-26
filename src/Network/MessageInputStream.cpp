/*
 * Copyright (c) 2013 Ghrum Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <Network/MessageInputStream.hpp>

using namespace Ghrum;

/////////////////////////////////////////////////////////////////
// {@see MessageInputStream::readBoolean} ///////////////////////
/////////////////////////////////////////////////////////////////
MessageInputStream::MessageInputStream(std::deque<int8_t> & buffer)
    : buffer_(buffer) {
}

/////////////////////////////////////////////////////////////////
// {@see MessageInputStream::readBoolean} ///////////////////////
/////////////////////////////////////////////////////////////////
bool MessageInputStream::readBoolean() {
    return (readByte() == 1 ? true : false);
}

/////////////////////////////////////////////////////////////////
// {@see MessageInputStream::readByte} //////////////////////////
/////////////////////////////////////////////////////////////////
int8_t MessageInputStream::readByte() {
    int8_t byte = buffer_.front();
    buffer_.pop_front();
    return byte;
}

/////////////////////////////////////////////////////////////////
// {@see MessageInputStream::readBytes} /////////////////////////
/////////////////////////////////////////////////////////////////
size_t MessageInputStream::readBytes(int8_t * buffer, size_t length) {
    if (buffer_.size() < length) {
        length = buffer_.size();
    }
    for (size_t i = 0; i < length; i++) {
        buffer[i] = readByte();
    }
    return length;
}

/////////////////////////////////////////////////////////////////
// {@see MessageInputStream::readUnsignedByte} //////////////////
/////////////////////////////////////////////////////////////////
uint8_t MessageInputStream::readUnsignedByte() {
    return (readByte() ^ 0x80) - 0x80;
}

/////////////////////////////////////////////////////////////////
// {@see MessageInputStream::readShort} /////////////////////////
/////////////////////////////////////////////////////////////////
int16_t MessageInputStream::readShort() {
    return int16_t(readUnsignedShort());
}

/////////////////////////////////////////////////////////////////
// {@see MessageInputStream::readUnsignedShort} /////////////////
/////////////////////////////////////////////////////////////////
uint16_t MessageInputStream::readUnsignedShort() {
    return (isBigEndianness_
            ? (uint16_t(readUnsignedByte()) << 8) | readUnsignedByte()
            : readUnsignedByte() | (uint16_t(readUnsignedByte()) << 8));
}

/////////////////////////////////////////////////////////////////
// {@see MessageInputStream::readInteger} ///////////////////////
/////////////////////////////////////////////////////////////////
int32_t MessageInputStream::readInteger() {
    return int32_t(readUnsignedInteger());
}

/////////////////////////////////////////////////////////////////
// {@see MessageInputStream::initialize} ////////////////////////
/////////////////////////////////////////////////////////////////
uint32_t MessageInputStream::readUnsignedInteger() {
    return (isBigEndianness_
            ? (uint32_t(readUnsignedShort()) << 16) | readUnsignedShort()
            : readUnsignedShort() | (uint32_t(readUnsignedShort()) << 16));
}

/////////////////////////////////////////////////////////////////
// {@see MessageInputStream::readLong} //////////////////////////
/////////////////////////////////////////////////////////////////
int64_t MessageInputStream::readLong() {
    return int64_t(readUnsignedLong());
}

/////////////////////////////////////////////////////////////////
// {@see MessageInputStream::readUnsignedLong} //////////////////
/////////////////////////////////////////////////////////////////
uint64_t MessageInputStream::readUnsignedLong() {
    return (isBigEndianness_
            ? (uint64_t(readUnsignedInteger()) << 32) | readUnsignedInteger()
            : readUnsignedInteger() | (uint64_t(readUnsignedInteger()) << 32));
}

/////////////////////////////////////////////////////////////////
// {@see MessageInputStream::readFloat} /////////////////////////
/////////////////////////////////////////////////////////////////
float MessageInputStream::readFloat() {
    union {
        uint32_t index;
        float value;
    } ieee_float;
    ieee_float.index = readUnsignedInteger();
    return ieee_float.value;
}

/////////////////////////////////////////////////////////////////
// {@see MessageInputStream::readDouble} ////////////////////////
/////////////////////////////////////////////////////////////////
double MessageInputStream::readDouble() {
    union {
        uint64_t index;
        double value;
    } ieee_double;
    ieee_double.index = readUnsignedLong();
    return ieee_double.value;
}

/////////////////////////////////////////////////////////////////
// {@see MessageInputStream::readString} ////////////////////////
/////////////////////////////////////////////////////////////////
std::string MessageInputStream::readString() {
    uint8_t length = readUnsignedByte();
    uint8_t buffer[length];
    readBytes(
        reinterpret_cast<int8_t *>(&buffer), length);
    buffer[length] = 0x00;
    return std::string((char *)&buffer);
}

/////////////////////////////////////////////////////////////////
// {@see MessageInputStream::readUnicode} ///////////////////////
/////////////////////////////////////////////////////////////////
std::u16string MessageInputStream::readUnicode() {
    uint32_t length = readUnsignedInteger();
    uint16_t buffer[length];
    readBytes(
        reinterpret_cast<int8_t *>(&buffer), length * 2);
    buffer[length] = 0x0000;
    return std::u16string((char16_t *)&buffer);
}

/////////////////////////////////////////////////////////////////
// {@see MessageInputStream::getLength} /////////////////////////
/////////////////////////////////////////////////////////////////
size_t MessageInputStream::getLength() {
    return buffer_.size();
}

/////////////////////////////////////////////////////////////////
// {@see MessageInputStream::skipBytes} /////////////////////////
/////////////////////////////////////////////////////////////////
void MessageInputStream::skipBytes(size_t length) {
    for (size_t i = 0; i < length; i++)
        buffer_.pop_front();
}

/////////////////////////////////////////////////////////////////
// {@see MessageInputStream::setEndianness} /////////////////////
/////////////////////////////////////////////////////////////////
void MessageInputStream::setEndianness(bool isBigEndianness) {
    isBigEndianness_ = isBigEndianness;
}

/////////////////////////////////////////////////////////////////
// {@see MessageInputStream::getBuffer} /////////////////////////
/////////////////////////////////////////////////////////////////
std::deque<int8_t> & MessageInputStream::getBuffer() {
    return buffer_;
}