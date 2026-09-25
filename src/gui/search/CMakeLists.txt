# SPDX-FileCopyrightText: 2026 Nextcloud GmbH and Nextcloud contributors
# SPDX-License-Identifier: GPL-2.0-or-later

set(CMAKE_AUTOMOC ON)

add_library(nextcloudGuiSearch STATIC)

foreach(_search_component_behavior_file UnifiedSearchFilterBar.qml UnifiedSearchPeoplePopup.qml)
  configure_file(${_search_component_behavior_file}.in ${CMAKE_CURRENT_SOURCE_DIR}/${_search_component_behavior_file})
endforeach()

if ("${QT_VERSION_MAJOR}.${QT_VERSION_MINOR}" VERSION_GREATER_EQUAL "6.8")
    set(QML_SEARCH_ACCESSIBILITY_STATUS_HANDLER [=[
        function onAccessibilityStatusChanged() {
            if (root.searchModel.accessibilityStatus.length > 0)
                Accessible.announce(root.searchModel.accessibilityStatus, Accessible.Polite)
        }
]=])
else()
    set(QML_SEARCH_ACCESSIBILITY_STATUS_HANDLER "")
endif()
configure_file(SearchWindow.qml.in ${CMAKE_CURRENT_SOURCE_DIR}/SearchWindow.qml)

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

  set(_search_qmldir "${CMAKE_CURRENT_BINARY_DIR}/qmldir")
  set(_search_qmldir_content "module com.nextcloud.desktopclient.search\n")
  string(APPEND _search_qmldir_content "optional plugin nextcloudGuiSearchplugin\n")
  string(APPEND _search_qmldir_content "classname com_nextcloud_desktopclient_searchPlugin\n")
  string(APPEND _search_qmldir_content "prefer :/qt/qml/com/nextcloud/desktopclient/search/\n")
  foreach(_search_qml_file IN LISTS _search_qml_files)
    get_filename_component(_search_qml_type "${_search_qml_file}" NAME_WE)
    string(APPEND _search_qmldir_content "${_search_qml_type} 254.0 ${_search_qml_file}\n")
  endforeach()
  file(WRITE "${_search_qmldir}" "${_search_qmldir_content}")
  set_source_files_properties("${_search_qmldir}" PROPERTIES QT_RESOURCE_ALIAS "qmldir")
  qt_add_resources(nextcloudGuiSearch search_qmldir
    PREFIX "/qt/qml/com/nextcloud/desktopclient/search"
    FILES "${_search_qmldir}"
  )

  file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/nextcloudGuiSearchplugin.cpp"
    "#include <QQmlEngineExtensionPlugin>\n"
    "#include <QtQml/qqmlmoduleregistration.h>\n"
    "#include <qqml.h>\n"
    "\n"
    "#include \"gui/search/unifiedsearchpeoplemodel.h\"\n"
    "#include \"gui/search/unifiedsearchresultslistmodel.h\"\n"
    "\n"
    "static void qml_register_types_com_nextcloud_desktopclient_search()\n"
    "{\n"
    "    qmlRegisterModule(\"com.nextcloud.desktopclient.search\", 1, 0);\n"
    "    qmlRegisterType<OCC::UnifiedSearchPeopleModel>(\"com.nextcloud.desktopclient.search\", 1, 0, \"UnifiedSearchPeopleModel\");\n"
    "    qmlRegisterType<OCC::UnifiedSearchResultsListModel>(\"com.nextcloud.desktopclient.search\", 1, 0, \"UnifiedSearchResultsListModel\");\n"
    "}\n"
    "static const QQmlModuleRegistration registration(\"com.nextcloud.desktopclient.search\", qml_register_types_com_nextcloud_desktopclient_search);\n"
    "\n"
    "class com_nextcloud_desktopclient_searchPlugin : public QQmlEngineExtensionPlugin\n"
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
      nextcloudCore
      Qt::Qml
  )
  set_target_properties(nextcloudGuiSearchplugin PROPERTIES
    AUTOMOC ON
    AUTOMOC_MOC_OPTIONS "-Muri=com.nextcloud.desktopclient.search"
  )
  # QT_STATICPLUGIN is required so that the moc-generated QT_MOC_EXPORT_PLUGIN
  # code emits the qt_static_plugin_*() function referenced by Q_IMPORT_QML_PLUGIN
  # in main.cpp.
  target_compile_definitions(nextcloudGuiSearchplugin PUBLIC QT_STATICPLUGIN)
endif()

target_link_libraries(nextcloudGuiSearch PRIVATE nextcloudCore)
target_compile_definitions(nextcloudGuiSearch PRIVATE QT_NO_KEYWORDS)
target_compile_definitions(nextcloudGuiSearchplugin PRIVATE QT_NO_KEYWORDS)
