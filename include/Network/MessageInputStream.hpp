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
#ifndef _MESSAGE_INPUT_STREAM_HPP_
#define _MESSAGE_INPUT_STREAM_HPP_

#include <Network/IMessageInputStream.hpp>
#include <deque>

namespace Ghrum {

/**
 * Implementation of {@see MessageInputStream}.
 *
 * @author Agustin Alvarez <wolftein@ghrum.org>
 */
class MessageInputStream : public IMessageInputStream {
public:
    /**
     * Default constructor of a input stream.
     *
     * @param buffer where the data is at.
     */
    MessageInputStream(std::deque<int8_t> & buffer);

    /**
     * {@inheritDoc}
     */
    bool readBoolean();

    /**
     * {@inheritDoc}
     */
    int8_t readByte();

    /**
     * {@inheritDoc}
     */
    size_t readBytes(int8_t * buffer, size_t length);

    /**
     * {@inheritDoc}
     */
    uint8_t readUnsignedByte();

    /**
     * {@inheritDoc}
     */
    int16_t readShort();

    /**
     * {@inheritDoc}
     */
    uint16_t readUnsignedShort();

    /**
     * {@inheritDoc}
     */
    int32_t readInteger();

    /**
     * {@inheritDoc}
     */
    uint32_t readUnsignedInteger();

    /**
     * {@inheritDoc}
     */
    int64_t readLong();

    /**
     * {@inheritDoc}
     */
    uint64_t readUnsignedLong();

    /**
     * {@inheritDoc}
     */
    float readFloat();

    /**
     * {@inheritDoc}
     */
    double readDouble();

    /**
     * {@inheritDoc}
     */
    std::string readString();

    /**
     * {@inheritDoc}
     */
    std::u16string readUnicode();

    /**
     * {@inheritDoc}
     */
    size_t getLength();

    /**
     * {@inheritDoc}
     */
    void skipBytes(size_t length);

    /**
     * {@inheritDoc}
     */
    void setEndianness(bool isBigEndianness);

    /**
     * {@inheritDoc}
     */
    std::deque<int8_t> & getBuffer();
private:
    bool isBigEndianness_;
    std::deque<int8_t> & buffer_;
};

} // namespace Ghrum

#endif // _MESSAGE_INPUT_STREAM_HPP_
