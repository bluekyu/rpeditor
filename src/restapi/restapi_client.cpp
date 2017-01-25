#include "restapi/restapi_client.hpp"

#include <QCoreApplication>

#include "restapi/resolve_message.hpp"
#include "restapi/restapi_event.hpp"
#include "main_window.hpp"

namespace rpeditor {
namespace restapi {

RestAPIClient::RestAPIClient(boost::asio::io_service& io_service,
    boost::asio::ip::tcp::resolver::iterator endpoint_iterator): io_service_(io_service), socket_(io_service)
{
    do_connect(endpoint_iterator);
}

void RestAPIClient::close(void)
{
    io_service_.post([this]() { socket_.close(); });
}

void RestAPIClient::do_connect(boost::asio::ip::tcp::resolver::iterator endpoint_iterator)
{
    boost::asio::async_connect(socket_, endpoint_iterator,
        [this](boost::system::error_code ec, boost::asio::ip::tcp::resolver::iterator)
    {
        if (!ec)
        {
            do_read_header();
        }
    }
    );
}

void RestAPIClient::do_read_header(void)
{
    boost::asio::async_read(socket_, boost::asio::buffer(&read_msg_.get_data().body_length, RestAPIMessage::HEADER_LENGTH),
        [this](boost::system::error_code ec, std::size_t /*length*/)
    {
        if (!ec && read_msg_.decode_header())
        {
            do_read_body();
        }
        else
        {
            socket_.close();
        }
    }
    );
}

void RestAPIClient::do_read_body(void)
{
    boost::asio::async_read(socket_,
        boost::asio::buffer(read_msg_.get_body(), read_msg_.get_body_length()),
        [this](boost::system::error_code ec, std::size_t /*length*/)
    {
        if (!ec)
        {
            QCoreApplication::postEvent(MainWindow::get_instance(), new RestAPIEvent(std::string(read_msg_.get_body(), read_msg_.get_body_length())));

            do_read_header();
        }
        else
        {
            socket_.close();
        }
    }
    );
}

void RestAPIClient::do_write(void)
{
    boost::asio::async_write(socket_,
        boost::asio::buffer(&write_msgs_.front().get_data(), write_msgs_.front().get_length()),
        [this](boost::system::error_code ec, std::size_t /*length*/)
    {
        if (!ec)
        {
            write_msgs_.pop_front();
            if (!write_msgs_.empty())
            {
                do_write();
            }
        }
        else
        {
            socket_.close();
        }
    }
    );
}

}   // namespace restapi
}   // namespace rpeditor
