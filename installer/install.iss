; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

#define MyAppName "Knossos"
#define MyAppVersion "4.0"
#define MyAppPublisher "Knossos"
#define MyAppURL "http://www.KnossosTool.org"
#define MyAppExeName "knossos.exe"
#define KNOSSOS_SRC_PATH "../"
#define License "../LICENSE"

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{7409D80B-C28D-4A51-9F78-A66C67830AB5}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
;AppVerName={#MyAppName} {#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={pf}\{#MyAppName}{#MyAppVersion}
DefaultGroupName={#MyAppName}
LicenseFile={#License}
OutputBaseFilename=knossos-setup{#MyAppVersion}
SetupIconFile=logo.ico
WizardSmallImageFile=logo.bmp       
WizardImageFile=bar.bmp
Compression=lzma
SolidCompression=yes


[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked


[Dirs]
Name: "{app}\doc"
Name: "{app}\platforms"
Name: "{app}\skeletonFiles"
Name: "{app}\sqldrivers"
Name: "{app}\task-files"                         

[Files]
Source: "{#KNOSSOS_SRC_PATH}knossos.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#KNOSSOS_SRC_PATH}icon"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#KNOSSOS_SRC_PATH}logo.ico"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#KNOSSOS_SRC_PATH}glut32.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#KNOSSOS_SRC_PATH}icudt51.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#KNOSSOS_SRC_PATH}icuin51.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#KNOSSOS_SRC_PATH}icuuc51.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#KNOSSOS_SRC_PATH}Qt5CLucene.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#KNOSSOS_SRC_PATH}Qt5Core.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#KNOSSOS_SRC_PATH}Qt5Gui.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#KNOSSOS_SRC_PATH}Qt5Help.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#KNOSSOS_SRC_PATH}Qt5Network.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#KNOSSOS_SRC_PATH}Qt5OpenGL.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#KNOSSOS_SRC_PATH}Qt5Sql.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#KNOSSOS_SRC_PATH}Qt5Test.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#KNOSSOS_SRC_PATH}Qt5Widgets.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#KNOSSOS_SRC_PATH}libcurl-4.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#KNOSSOS_SRC_PATH}libgcc_s_dw2-1.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#KNOSSOS_SRC_PATH}libpython2.7.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#KNOSSOS_SRC_PATH}libstdc++-6.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#KNOSSOS_SRC_PATH}libwinpthread-1.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#KNOSSOS_SRC_PATH}PythonQt.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#KNOSSOS_SRC_PATH}splash"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#KNOSSOS_SRC_PATH}doc\*"; DestDir: "{app}\doc"; Flags: ignoreversion recursesubdirs createallsubdirs
; platforms and sqldrivers are needed by QT
Source: "{#KNOSSOS_SRC_PATH}platforms\*"; DestDir: "{app}\platforms"; Flags: ignoreversion recursesubdirs createallsubdirs 
Source: "{#KNOSSOS_SRC_PATH}sqldrivers\*"; DestDir: "{app}\sqldrivers"; Flags: ignoreversion recursesubdirs createallsubdirs
; NOTE: Don't use "Flags: ignoreversion" on any shared system files
[UninstallDelete]
Type: files; Name: "{app}"

[Icons]
Name: "{group}\{#MyAppName}"; Filename: "{app}\knossos.exe" ; IconFilename: "{app}\logo.ico"
Name: "{group}\Uninstall"; Filename: "{uninstallexe}"
Name: "{commondesktop}\{#MyAppName}"; Filename: "{app}\knossos.exe"; Tasks: desktopicon ; IconFilename: "{app}\logo.ico"
       

;Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, '&', '&&')}}"; Flags: postinstall skipifsilent

