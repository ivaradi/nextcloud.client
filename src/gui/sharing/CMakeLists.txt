# SPDX-FileCopyrightText: 2026 Nextcloud GmbH and Nextcloud contributors
# SPDX-License-Identifier: GPL-2.0-or-later

set(CMAKE_AUTOMOC ON)

add_library(nextcloudGuiSharing STATIC)

if ("${QT_VERSION_MAJOR}.${QT_VERSION_MINOR}" VERSION_GREATER_EQUAL "6.5")
    set(QML_AVATAR_EFFECT_IMPORT "import QtQuick.Effects")
    set(QML_AVATAR_EFFECT [=[
    MultiEffect {
        objectName: "recipientAvatarEffect"

        anchors.fill: parent
        source: avatarImage
        maskEnabled: true
        maskSource: maskShape
    }
]=])
else()
    set(QML_AVATAR_EFFECT_IMPORT "import Qt5Compat.GraphicalEffects")
    set(QML_AVATAR_EFFECT [=[
    OpacityMask {
        objectName: "recipientAvatarEffect"

        anchors.fill: parent
        source: avatarImage
        maskSource: maskShape
    }
]=])
endif()
configure_file(RecipientAvatar.qml.in ${CMAKE_CURRENT_SOURCE_DIR}/RecipientAvatar.qml)

foreach(_sharing_component_behavior_file FieldDelegate.qml ShareAdvancedSettingsPage.qml ShareDetailsFrame.qml ShareDialog.qml ShareListPage.qml)
  configure_file(${_sharing_component_behavior_file}.in ${CMAKE_CURRENT_SOURCE_DIR}/${_sharing_component_behavior_file})
endforeach()

set_source_files_properties(RecipientIcon.qml PROPERTIES QT_QML_SINGLETON_TYPE TRUE)

target_sources(nextcloudGuiSharing
  PRIVATE
    sharedetailslistmodel.h
    sharedetailslistmodel.cpp
    sharingconstants.h
    unifiedsharingrequest.h
    unifiedsharingrequest.cpp
    createsharejob.h
    createsharejob.cpp
    deletesharejob.h
    deletesharejob.cpp
    generatesecretjob.h
    generatesecretjob.cpp
    getsharejob.h
    getsharejob.cpp
    getsharesjob.h
    getsharesjob.cpp
    searchrecipientsjob.h
    searchrecipientsjob.cpp
    updatesharejob.h
    updatesharejob.cpp
    addsourcejob.h
    addsourcejob.cpp
    removesourcejob.h
    removesourcejob.cpp
    addrecipientjob.h
    addrecipientjob.cpp
    removerecipientjob.h
    removerecipientjob.cpp
    setpropertyjob.h
    setpropertyjob.cpp
    setpermissionjob.h
    setpermissionjob.cpp
    setrecipientpermissionjob.h
    setrecipientpermissionjob.cpp
    setpermissionpresetjob.h
    setpermissionpresetjob.cpp
    setrecipientsecretjob.h
    setrecipientsecretjob.cpp
    setsharestatejob.h
    setsharestatejob.cpp
    permission.h
    permission.cpp
    permissionmodel.h
    permissionmodel.cpp
    property.h
    property.cpp
    propertymodel.h
    propertymodel.cpp
    recipient.h
    recipient.cpp
    recipienticonutils.h
    recipienticonutils.cpp
    recipientmodel.h
    recipientmodel.cpp
    recipientsearchmodel.h
    recipientsearchmodel.cpp
    unifiedshare.h
    unifiedshare.cpp
    sharingcontroller.h
    sharingcontroller.cpp
    unifiedsharelistmodel.h
    unifiedsharelistmodel.cpp
)

if(ECM_VERSION VERSION_GREATER_EQUAL "6.0.0")
  ecm_add_qml_module(nextcloudGuiSharing
    URI com.nextcloud.desktopclient.sharing
    GENERATE_PLUGIN_SOURCE
    QML_FILES
      FieldDelegate.qml
      PermissionList.qml
      RecipientPermissionDialog.qml
      DeleteShareConfirmation.qml
      RecipientAvatar.qml
      RecipientIcon.qml
      RecipientSearchField.qml
      ShareActionRow.qml
      ShareDetailsPage.qml
      ShareAdvancedSettingsPage.qml
      ShareDetailsFrame.qml
      ShareListPage.qml
      ShareRow.qml
      SettingsPanel.qml
      ShareDialog.qml
  )
else()
  set(_sharing_qml_files
    FieldDelegate.qml
    PermissionList.qml
    RecipientPermissionDialog.qml
    DeleteShareConfirmation.qml
    RecipientAvatar.qml
    RecipientIcon.qml
    RecipientSearchField.qml
    ShareActionRow.qml
    ShareDetailsPage.qml
    ShareAdvancedSettingsPage.qml
    ShareDetailsFrame.qml
    ShareListPage.qml
    ShareRow.qml
    SettingsPanel.qml
    ShareDialog.qml
  )

  qt_add_resources(nextcloudGuiSharing sharing_qml
    PREFIX "/qt/qml/com/nextcloud/desktopclient/sharing"
    BASE "${CMAKE_CURRENT_SOURCE_DIR}"
    FILES ${_sharing_qml_files}
  )

  set(_sharing_qmldir "${CMAKE_CURRENT_BINARY_DIR}/qmldir")
  set(_sharing_qmldir_content "module com.nextcloud.desktopclient.sharing\n")
  string(APPEND _sharing_qmldir_content "optional plugin nextcloudGuiSharingplugin\n")
  string(APPEND _sharing_qmldir_content "classname com_nextcloud_desktopclient_sharingPlugin\n")
  string(APPEND _sharing_qmldir_content "prefer :/qt/qml/com/nextcloud/desktopclient/sharing/\n")
  foreach(_sharing_qml_file IN LISTS _sharing_qml_files)
    get_filename_component(_sharing_qml_type "${_sharing_qml_file}" NAME_WE)
    if(_sharing_qml_file STREQUAL "RecipientIcon.qml")
      string(APPEND _sharing_qmldir_content "singleton ${_sharing_qml_type} 254.0 ${_sharing_qml_file}\n")
    else()
      string(APPEND _sharing_qmldir_content "${_sharing_qml_type} 254.0 ${_sharing_qml_file}\n")
    endif()
  endforeach()
  file(WRITE "${_sharing_qmldir}" "${_sharing_qmldir_content}")
  set_source_files_properties("${_sharing_qmldir}" PROPERTIES QT_RESOURCE_ALIAS "qmldir")
  qt_add_resources(nextcloudGuiSharing sharing_qmldir
    PREFIX "/qt/qml/com/nextcloud/desktopclient/sharing"
    FILES "${_sharing_qmldir}"
  )

  file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/nextcloudGuiSharingplugin.cpp"
    "#include <QQmlEngineExtensionPlugin>\n"
    "#include <QtQml/qqmlmoduleregistration.h>\n"
    "#include <qqml.h>\n"
    "\n"
    "#include \"gui/sharing/permissionmodel.h\"\n"
    "#include \"gui/sharing/propertymodel.h\"\n"
    "#include \"gui/sharing/recipient.h\"\n"
    "#include \"gui/sharing/recipientmodel.h\"\n"
    "#include \"gui/sharing/recipientsearchmodel.h\"\n"
    "#include \"gui/sharing/sharingcontroller.h\"\n"
    "#include \"gui/sharing/unifiedshare.h\"\n"
    "#include \"gui/sharing/unifiedsharelistmodel.h\"\n"
    "\n"
    "static void qml_register_types_com_nextcloud_desktopclient_sharing()\n"
    "{\n"
    "    qmlRegisterModule(\"com.nextcloud.desktopclient.sharing\", 1, 0);\n"
    "    qmlRegisterType<OCC::Gui::Sharing::PermissionModel>(\"com.nextcloud.desktopclient.sharing\", 1, 0, \"PermissionModel\");\n"
    "    qmlRegisterType<OCC::Gui::Sharing::PropertyModel>(\"com.nextcloud.desktopclient.sharing\", 1, 0, \"PropertyModel\");\n"
    "    qmlRegisterUncreatableType<OCC::Gui::Sharing::Recipient>(\"com.nextcloud.desktopclient.sharing\", 1, 0, \"Recipient\", QStringLiteral(\"created by SharingController\"));\n"
    "    qmlRegisterType<OCC::Gui::Sharing::RecipientModel>(\"com.nextcloud.desktopclient.sharing\", 1, 0, \"RecipientModel\");\n"
    "    qmlRegisterType<OCC::Gui::Sharing::RecipientSearchModel>(\"com.nextcloud.desktopclient.sharing\", 1, 0, \"RecipientSearchModel\");\n"
    "    qmlRegisterUncreatableType<OCC::Gui::Sharing::Share>(\"com.nextcloud.desktopclient.sharing\", 1, 0, \"Share\", QStringLiteral(\"created via SharingController\"));\n"
    "    qmlRegisterType<OCC::Gui::Sharing::SharingController>(\"com.nextcloud.desktopclient.sharing\", 1, 0, \"SharingController\");\n"
    "    qmlRegisterType<OCC::Gui::Sharing::UnifiedShareListModel>(\"com.nextcloud.desktopclient.sharing\", 1, 0, \"UnifiedShareListModel\");\n"
    "}\n"
    "static const QQmlModuleRegistration registration(\"com.nextcloud.desktopclient.sharing\", qml_register_types_com_nextcloud_desktopclient_sharing);\n"
    "\n"
    "class com_nextcloud_desktopclient_sharingPlugin : public QQmlEngineExtensionPlugin\n"
    "{\n"
    "    Q_OBJECT\n"
    "    Q_PLUGIN_METADATA(IID \"org.qt-project.Qt.QQmlEngineExtensionInterface\")\n"
    "};\n"
    "\n"
    "#include \"nextcloudGuiSharingplugin.moc\"\n"
  )
  add_library(nextcloudGuiSharingplugin STATIC)
  target_sources(nextcloudGuiSharingplugin PRIVATE
    "${CMAKE_CURRENT_BINARY_DIR}/nextcloudGuiSharingplugin.cpp"
  )
  target_link_libraries(nextcloudGuiSharingplugin
    PUBLIC
      nextcloudGuiSharing
    PRIVATE
      nextcloudCore
      Qt::Qml
  )
  set_target_properties(nextcloudGuiSharingplugin PROPERTIES
    AUTOMOC ON
    AUTOMOC_MOC_OPTIONS "-Muri=com.nextcloud.desktopclient.sharing"
  )
  # QT_STATICPLUGIN is required so that the moc-generated QT_MOC_EXPORT_PLUGIN
  # code emits the qt_static_plugin_*() function referenced by Q_IMPORT_QML_PLUGIN
  # in main.cpp.
  target_compile_definitions(nextcloudGuiSharingplugin PUBLIC QT_STATICPLUGIN)
endif()

target_link_libraries(nextcloudGuiSharing PRIVATE nextcloudCore)
target_compile_definitions(nextcloudGuiSharing PRIVATE QT_NO_KEYWORDS)
target_compile_definitions(nextcloudGuiSharingplugin PRIVATE QT_NO_KEYWORDS)
