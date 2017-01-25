#pragma once

#include <cstdint>
#include <type_traits>

namespace rpeditor {
namespace restapi {

class RestAPIMessage
{
public:
    struct DataType
    {
        uint32_t body_length;   // header
        char body[4096];        // body
    };
    static_assert(std::is_pod<DataType>::value, "RestAPIMessage::DataType is NOT POD type.");

#if _MSC_VER >= 1900
    static const size_t HEADER_LENGTH = sizeof(DataType::body_length);
    static const size_t MAX_BODY_LENGTH = std::extent<decltype(DataType::body)>::value;
#else
    static const size_t HEADER_LENGTH = sizeof(DataType().body_length);
    static const size_t MAX_BODY_LENGTH = std::extent<decltype(DataType().body)>::value;
#endif

public:
    RestAPIMessage(void);

    const DataType& get_data(void) const;
    DataType& get_data(void);

    std::size_t get_length(void) const;

    const char* get_body(void) const;
    char* get_body(void);

    std::size_t get_body_length(void) const;

    void set_body_length(std::uint32_t new_length);

    bool decode_header(void);
    void encode_header(void);

private:
    DataType data_;
};

// ************************************************************************************************

inline RestAPIMessage::RestAPIMessage(void)
{
    data_.body_length = 0;
}

inline const RestAPIMessage::DataType& RestAPIMessage::get_data(void) const
{
    return data_;
}

inline RestAPIMessage::DataType& RestAPIMessage::get_data(void)
{
    return data_;
}

inline std::size_t RestAPIMessage::get_length(void) const
{
    return HEADER_LENGTH + data_.body_length;
}

inline const char* RestAPIMessage::get_body(void) const
{
    return data_.body;
}

inline char* RestAPIMessage::get_body(void)
{
    return data_.body;
}

inline std::size_t RestAPIMessage::get_body_length(void) const
{
    return data_.body_length;
}

inline void RestAPIMessage::set_body_length(std::uint32_t new_length)
{
    data_.body_length = new_length < MAX_BODY_LENGTH ? new_length : MAX_BODY_LENGTH;
}

inline bool RestAPIMessage::decode_header(void)
{
    if (data_.body_length > MAX_BODY_LENGTH)
    {
        data_.body_length = 0;
        return false;
    }
    return true;
}

inline void RestAPIMessage::encode_header(void)
{
    // validate body length.
    set_body_length(data_.body_length);
}

}   // namespace restapi
}   // namespace rpeditor
