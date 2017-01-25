#pragma once

#include <deque>

#include <boost/asio.hpp>

#include "restapi_message.hpp"

namespace rpeditor {
namespace restapi {

class RestAPIClient
{
public:
    RestAPIClient(boost::asio::io_service& io_service, boost::asio::ip::tcp::resolver::iterator endpoint_iterator);

    void write(const std::string& json_msg);

    void close(void);

private:
    void do_connect(boost::asio::ip::tcp::resolver::iterator endpoint_iterator);

    void do_read_header(void);

    void do_read_body(void);

    void do_write(void);

    boost::asio::io_service& io_service_;
    boost::asio::ip::tcp::socket socket_;
    RestAPIMessage read_msg_;
    std::deque<RestAPIMessage> write_msgs_;
};

// ************************************************************************************************

inline void RestAPIClient::write(const std::string& json_msg)
{
    RestAPIMessage msg;
    msg.set_body_length(static_cast<uint32_t>(json_msg.length()));
    std::memcpy(msg.get_body(), json_msg.c_str(), msg.get_body_length());
    msg.encode_header();

    io_service_.post([this, msg]() {
        bool write_in_progress = !write_msgs_.empty();
        write_msgs_.push_back(msg);
        if (!write_in_progress)
        {
            do_write();
        }
    });
}

}   // namespace restapi
}   // namespace rpeditor
