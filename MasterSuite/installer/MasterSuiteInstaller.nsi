!define APPNAME "MasterSuite"
!define COMPANY "MyCompany"
!define VERSION "1.0"
!define VST3_NAME "MasterSuite.vst3"
!define INSTFILE "${VST3_NAME}"
!define VST3PATH "$PROGRAMFILES\\Common Files\\VST3"

OutFile "MasterSuite_Installer.exe"
InstallDir "$INSTDIR"

ShowInstDetails show
ShowUninstDetails show

Page directory
Page instfiles
UninstPage uninstConfirm
UninstPage instfiles

Section "Install"
    SetOutPath "$INSTDIR"
    ; Expect the VST3 to be present next to the script during build (CI copies it there)
    File /oname=$INSTDIR\\${VST3_NAME} "${VST3_NAME}"

    ; Ensure system VST3 folder exists
    ${If} ${DirExists}("${VST3PATH}")
      ; folder exists
    ${Else}
      CreateDirectory "${VST3PATH}"
    ${EndIf}

    ; Copy VST3 to common VST3 location
    CopyFiles "$INSTDIR\\${VST3_NAME}" "${VST3PATH}\\${VST3_NAME}"

    ; Write uninstaller
    WriteUninstaller "$INSTDIR\\Uninstall.exe"

    ; create uninstall shortcut in Start Menu
    CreateShortCut "$SMPROGRAMS\\${APPNAME}\\Uninstall.lnk" "$INSTDIR\\Uninstall.exe"
SectionEnd

Section "Uninstall"
    Delete "${VST3PATH}\\${VST3_NAME}"
    Delete "$INSTDIR\\${VST3_NAME}"
    Delete "$INSTDIR\\Uninstall.exe"
    Delete "$SMPROGRAMS\\${APPNAME}\\Uninstall.lnk"
    RMDir /r "$INSTDIR"
    RMDir "$SMPROGRAMS\\${APPNAME}"
SectionEnd
