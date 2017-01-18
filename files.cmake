# list src/
set(ui_src
   "${PROJECT_SOURCE_DIR}/src/main_window.ui"
)
qt5_wrap_ui(source_ui ${ui_src})

set(source_src_restapi
    "${PROJECT_SOURCE_DIR}/src/restapi/config.hpp"
    "${PROJECT_SOURCE_DIR}/src/restapi/showbase.cpp"
    "${PROJECT_SOURCE_DIR}/src/restapi/showbase.hpp"
)

set(source_src
    "${PROJECT_SOURCE_DIR}/src/logging.cpp"
    "${PROJECT_SOURCE_DIR}/src/logging.hpp"
    "${PROJECT_SOURCE_DIR}/src/main.cpp"
    "${PROJECT_SOURCE_DIR}/src/main_window.cpp"
    "${PROJECT_SOURCE_DIR}/src/main_window.hpp"
    "${PROJECT_SOURCE_DIR}/src/restapi_client.cpp"
    "${PROJECT_SOURCE_DIR}/src/restapi_client.hpp"
    "${PROJECT_SOURCE_DIR}/src/restapi_message.hpp"
)

source_group("ui" FILES ${ui_src} ${source_ui})
source_group("src\\restapi" FILES ${source_src_restapi})
source_group("src" FILES ${source_src})

set(rpeditor_sources
    ${source_ui}
    ${source_src_restapi}
    ${source_src}
)
