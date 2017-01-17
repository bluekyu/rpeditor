# list src/
set(ui_src
   "${PROJECT_SOURCE_DIR}/src/main_window.ui"
   "${PROJECT_SOURCE_DIR}/src/options_dialog.ui"
)
qt5_wrap_ui(source_ui ${ui_src})

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
source_group("src\\qt" FILES ${source_qt})
source_group("src" FILES ${source_src})

set(rpeditor_sources
    ${source_ui}
    ${source_src}
)
