project(QuickRender)
find_package(MITK REQUIRED)

mitk_create_executable(DEPENDS MitkQmlItems)

file(COPY ${Qt5Core_DIR}/../../../plugins/platforms
  DESTINATION ${MITK_CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Debug)
file(COPY ${Qt5Core_DIR}/../../../qml/QtQuick.2
  DESTINATION ${MITK_CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Debug)
