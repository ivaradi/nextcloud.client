# SPDX-FileCopyrightText: 2026 Nextcloud GmbH and Nextcloud contributors
# SPDX-License-Identifier: GPL-2.0-or-later

set(CMAKE_AUTOMOC ON)

add_library(nextcloudGuiSearch STATIC)

target_sources(nextcloudGuiSearch
  PRIVATE
    unifiedsearchresult.h
    unifiedsearchresult.cpp
    unifiedsearchresultslistmodel.h
    unifiedsearchresultslistmodel.cpp
    unifiedsearchpeoplemodel.h
    unifiedsearchpeoplemodel.cpp
)

if(ECM_VERSION VERSION_GREATER_EQUAL "6.0.0")
  ecm_add_qml_module(nextcloudGuiSearch
    URI com.nextcloud.desktopclient.search
    GENERATE_PLUGIN_SOURCE
    QML_FILES
      SearchWindow.qml
      UnifiedSearchInputContainer.qml
      UnifiedSearchResultFetchMoreTrigger.qml
      UnifiedSearchResultItem.qml
      UnifiedSearchResultItemSkeleton.qml
      UnifiedSearchResultItemSkeletonContainer.qml
      UnifiedSearchResultItemSkeletonGradientRectangle.qml
      UnifiedSearchResultListItem.qml
      UnifiedSearchResultNothingFound.qml
      UnifiedSearchPlaceholderView.qml
      UnifiedSearchResultSectionItem.qml
  )
else()
  # Workaround for CMake AUTOMOC bug with static libraries on Qt 6.2.x.
  # AUTOMOC doesn't properly propagate moc symbols from static libraries.
  # See: https://gitlab.kitware.com/cmake/cmake/-/issues/19939
  if(QT_VERSION_MAJOR STREQUAL "6" AND QT_VERSION_MINOR LESS 5)
    qt_generate_moc(unifiedsearchresultslistmodel.h ${CMAKE_CURRENT_BINARY_DIR}/moc_unifiedsearchresultslistmodel.cpp)
    target_sources(nextcloudGuiSearch PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/moc_unifiedsearchresultslistmodel.cpp)
    target_include_directories(nextcloudGuiSearch PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
  endif()

  set(_search_qml_files
    SearchWindow.qml
    UnifiedSearchCustomDateRangeDialog.qml
    UnifiedSearchDetailHeader.qml
    UnifiedSearchFilterBar.qml
    UnifiedSearchInputContainer.qml
    UnifiedSearchPeopleDelegate.qml
    UnifiedSearchPeoplePopup.qml
    UnifiedSearchResultDelegate.qml
    UnifiedSearchResultFetchMoreTrigger.qml
    UnifiedSearchResultItem.qml
    UnifiedSearchResultItemSkeleton.qml
    UnifiedSearchResultItemSkeletonContainer.qml
    UnifiedSearchResultItemSkeletonGradientRectangle.qml
    UnifiedSearchResultListItem.qml
    UnifiedSearchResultNothingFound.qml
    UnifiedSearchPlaceholderView.qml
    UnifiedSearchResultSectionItem.qml
  )

  qt_add_resources(nextcloudGuiSearch search_qml
    PREFIX "/qt/qml/com/nextcloud/desktopclient/search"
    BASE "${CMAKE_CURRENT_SOURCE_DIR}"
    FILES ${_search_qml_files}
  )

  file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/nextcloudGuiSearchplugin.cpp"
    "#include <QQmlEngineExtensionPlugin>\n"
    "\n"
    "class NextcloudGuiSearchPlugin : public QQmlEngineExtensionPlugin\n"
    "{\n"
    "    Q_OBJECT\n"
    "    Q_PLUGIN_METADATA(IID \"org.qt-project.Qt.QQmlEngineExtensionInterface\")\n"
    "};\n"
    "\n"
    "#include \"nextcloudGuiSearchplugin.moc\"\n"
  )
  add_library(nextcloudGuiSearchplugin STATIC)
  target_sources(nextcloudGuiSearchplugin PRIVATE
    "${CMAKE_CURRENT_BINARY_DIR}/nextcloudGuiSearchplugin.cpp"
  )
  target_link_libraries(nextcloudGuiSearchplugin
    PUBLIC
      nextcloudGuiSearch
    PRIVATE
      Qt::Qml
  )
  set_target_properties(nextcloudGuiSearchplugin PROPERTIES
    AUTOMOC ON
    AUTOMOC_MOC_OPTIONS "-Muri=com.nextcloud.desktopclient.search"
  )
endif()

target_link_libraries(nextcloudGuiSearch PRIVATE nextcloudCore)
target_compile_definitions(nextcloudGuiSearch PRIVATE QT_NO_KEYWORDS)
target_compile_definitions(nextcloudGuiSearchplugin PRIVATE QT_NO_KEYWORDS)
