#pragma once

#include <boost/log/sinks/basic_sink_backend.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>

namespace rpeditor {

extern boost::log::formatter log_format;

void initialize_logging(void);

}   // namespace rpeditor
