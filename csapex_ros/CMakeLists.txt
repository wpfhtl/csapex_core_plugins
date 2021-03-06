cmake_minimum_required(VERSION 2.8.3)
project(csapex_ros)

## Find catkin macros and libraries
## if COMPONENTS list like find_package(catkin REQUIRED COMPONENTS xyz)
## is used, also find other catkin packages
find_package(catkin COMPONENTS roscpp csapex csapex_core_plugins rosbag tf pluginlib REQUIRED)

find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

catkin_package(
  INCLUDE_DIRS include
  LIBRARIES csapex_ros_core
  CATKIN_DEPENDS pluginlib csapex_core_plugins
)

###########
## Build ##
###########

## Specify additional locations of header files
## Your package locations should be listed before other locations
include_directories(include ${catkin_INCLUDE_DIRS} ${Qt5Core_INCLUDE_DIRS} ${Qt5Widgets_INCLUDE_DIRS} ${Qt5Gui_INCLUDE_DIRS})

## booting
add_library(csapex_ros_boot
    src/ros_boot.cpp
)
target_link_libraries(csapex_ros_boot
  ${catkin_LIBRARIES}
)

add_custom_command(TARGET csapex_ros_boot POST_BUILD
    COMMAND mkdir -p ${CSAPEX_BOOT_PLUGIN_DIR} && ln -sf ${CATKIN_DEVEL_PREFIX}/lib/libcsapex_ros_boot.so ${CSAPEX_BOOT_PLUGIN_DIR}/libcsapex_ros_boot.so)

## extensions
add_library(csapex_ros_core
    src/generic_ros_message.cpp

    src/ros_node.cpp
    src/ros_handler.cpp
    src/ros_message_conversion.cpp
    src/tf_listener.cpp
)

target_link_libraries(csapex_ros_core
  ${catkin_LIBRARIES}
)

## core plugins
add_library(csapex_ros_plugin_core
    src/apex_ros_interface.cpp
    src/bag_provider.cpp
)

## Specify libraries to link a library or executable target against
target_link_libraries(csapex_ros_plugin_core csapex_ros_core
  ${catkin_LIBRARIES}
)

## node plugins
add_library(csapex_ros_plugin_node
    src/ros_param.cpp
    src/export_ros.cpp
    src/import_ros.cpp
    src/ros_test.cpp
    src/ros_trigger.cpp
    src/bag_writer.cpp
)

## Specify libraries to link a library or executable target against
target_link_libraries(csapex_ros_plugin_node csapex_ros_core
  ${catkin_LIBRARIES}
)

## qt plugins
add_library(csapex_ros_plugin_qt
    src/file_drag_io.cpp
)

## Specify libraries to link a library or executable target against
target_link_libraries(csapex_ros_plugin_qt csapex_ros_core
  ${catkin_LIBRARIES}
)


#
# INSTALL
#

install(FILES plugins.xml
        DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION})
install(TARGETS csapex_ros_boot
        ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
        LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
        RUNTIME DESTINATION ${CATKIN_GLOBAL_BIN_DESTINATION})
install(TARGETS csapex_ros_core
        ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
        LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
        RUNTIME DESTINATION ${CATKIN_GLOBAL_BIN_DESTINATION})
install(TARGETS csapex_ros_plugin_core
        ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
        LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
        RUNTIME DESTINATION ${CATKIN_GLOBAL_BIN_DESTINATION})
install(TARGETS csapex_ros_plugin_node
        ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
        LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
        RUNTIME DESTINATION ${CATKIN_GLOBAL_BIN_DESTINATION})
install(TARGETS csapex_ros_plugin_qt
        ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
        LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
        RUNTIME DESTINATION ${CATKIN_GLOBAL_BIN_DESTINATION})
