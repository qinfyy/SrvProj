#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

class Bitset
{
public:
    Bitset();
    explicit Bitset(std::string_view bytes);

    bool IsEmpty() const;
    void Clear();
    bool IsSet(uint32_t index) const;
    void SetBit(uint32_t index);
    void UnsetBit(uint32_t index);

    std::string ToByteArray() const;
    std::string ToBigEndianByteArray() const;

private:
    void EnsureIndex(uint32_t index);

    std::vector<uint64_t> mData;
};

inline Bitset::Bitset() : mData(1, 0)
{
}

inline Bitset::Bitset(std::string_view bytes) : mData(1, 0)
{
    if (bytes.empty())
    {
        return;
    }

    mData.assign((bytes.size() + 7) / 8, 0);
    for (size_t offset = 0; offset < bytes.size(); offset += 8)
    {
        const size_t blockSize = (std::min<size_t>)(8, bytes.size() - offset);
        for (size_t i = 0; i < blockSize; ++i)
        {
            const auto value = static_cast<uint64_t>(static_cast<uint8_t>(bytes[offset + i]));
            mData[offset / 8] |= value << ((blockSize - 1 - i) * 8);
        }
    }
}

inline bool Bitset::IsEmpty() const
{
    return mData.size() == 1 && mData[0] == 0;
}

inline void Bitset::Clear()
{
    mData.assign(1, 0);
}

inline bool Bitset::IsSet(uint32_t index) const
{
    if (index == 0)
    {
        return false;
    }

    const uint32_t zeroBased = index - 1;
    const size_t offset = zeroBased / 64;
    if (offset >= mData.size())
    {
        return false;
    }

    const uint32_t bit = zeroBased % 64;
    const uint64_t flag = uint64_t{1} << bit;
    return (mData[offset] & flag) == flag;
}

inline void Bitset::SetBit(uint32_t index)
{
    if (index == 0)
    {
        return;
    }

    EnsureIndex(index);
    const uint32_t zeroBased = index - 1;
    mData[zeroBased / 64] |= uint64_t{1} << (zeroBased % 64);
}

inline void Bitset::UnsetBit(uint32_t index)
{
    if (index == 0)
    {
        return;
    }

    EnsureIndex(index);
    const uint32_t zeroBased = index - 1;
    mData[zeroBased / 64] &= ~(uint64_t{1} << (zeroBased % 64));
}

inline std::string Bitset::ToByteArray() const
{
    std::string out;
    out.resize(mData.size() * 8);

    for (size_t i = 0; i < mData.size(); ++i)
    {
        uint64_t value = mData[i];
        for (int x = 7; x >= 0; --x)
        {
            out[(i * 8) + x] = static_cast<char>(value & 0xFF);
            value >>= 8;
        }
    }

    return out;
}

inline std::string Bitset::ToBigEndianByteArray() const
{
    std::string out;
    out.resize(mData.size() * 8);

    for (size_t i = 0; i < mData.size(); ++i)
    {
        uint64_t value = mData[i];
        for (int x = 0; x <= 7; ++x)
        {
            out[(i * 8) + x] = static_cast<char>(value & 0xFF);
            value >>= 8;
        }
    }

    return out;
}

inline void Bitset::EnsureIndex(uint32_t index)
{
    const size_t offset = (index - 1) / 64;
    if (offset >= mData.size())
    {
        mData.resize(offset + 1, 0);
    }
}
