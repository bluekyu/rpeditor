# list src/
set(ui_src
   "${PROJECT_SOURCE_DIR}/src/main_window.ui"
)
qt5_wrap_ui(source_ui ${ui_src})

set(source_src_restapi_resources
    "${PROJECT_SOURCE_DIR}/src/restapi/resources/nodepath.cpp"
    "${PROJECT_SOURCE_DIR}/src/restapi/resources/showbase.cpp"
)

set(source_src_restapi
    "${PROJECT_SOURCE_DIR}/src/restapi/config.hpp"
    "${PROJECT_SOURCE_DIR}/src/restapi/resolve_message.cpp"
    "${PROJECT_SOURCE_DIR}/src/restapi/resolve_message.hpp"
    "${PROJECT_SOURCE_DIR}/src/restapi/restapi_client.cpp"
    "${PROJECT_SOURCE_DIR}/src/restapi/restapi_client.hpp"
    "${PROJECT_SOURCE_DIR}/src/restapi/restapi_event.hpp"
)

set(source_src
    "${PROJECT_SOURCE_DIR}/src/collapsible_widget.cpp"
    "${PROJECT_SOURCE_DIR}/src/collapsible_widget.hpp"
    "${PROJECT_SOURCE_DIR}/src/logging.cpp"
    "${PROJECT_SOURCE_DIR}/src/logging.hpp"
    "${PROJECT_SOURCE_DIR}/src/main.cpp"
    "${PROJECT_SOURCE_DIR}/src/main_window.cpp"
    "${PROJECT_SOURCE_DIR}/src/main_window.hpp"
    "${PROJECT_SOURCE_DIR}/src/main_window_api.cpp"
)

source_group("ui" FILES ${ui_src} ${source_ui})
source_group("src\\restapi\\resources" FILES ${source_src_restapi_resources})
source_group("src\\restapi" FILES ${source_src_restapi})
source_group("src" FILES ${source_src})

set(rpeditor_sources
    ${source_ui}
    ${source_src_restapi_resources}
    ${source_src_restapi}
    ${source_src}
)
