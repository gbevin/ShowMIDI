; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

#define MyAppName "ShowMIDI"
#define MyAppPublisher "Uwyn"
#define MyAppURL "https://www.uwyn.com"
#define VSTFolder "{commoncf64}\VST\"
#define VST3Folder "{commoncf64}\VST3\"
#define CLAPFolder "{commoncf64}\CLAP\"

[Setup]
; NOTE: The value of AppId uniquely identifies this application. Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{CF5E95CF-E92F-4C46-913D-A78713153914}
AppName={#MyAppName}
AppVersion={#VERSION}
AppVerName={#MyAppName} v{#VERSION}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={commonpf}\{#MyAppPublisher}\{#MyAppName}
DefaultGroupName={#MyAppPublisher}\{#MyAppName}
LicenseFile=gpl-3.0.txt
Compression=lzma
SolidCompression=yes
OutputDir={#PWD}\Installers\
ArchitecturesInstallIn64BitMode=x64
OutputBaseFilename=ShowMIDI-Windows-{#VERSION}
SetupLogging=yes

DisableWelcomePage= no
DisableProgramGroupPage=no
UsePreviousSetupType = no

WizardImageFile=wizard_image*.bmp
WizardSmallImageFile=wizard_icon*.bmp
WizardImageStretch=no

[Types]
Name: "custom"; Description: "Custom installation"; Flags: iscustom

[Components]
Name: "Standalone"; Description: "Standalone application"; Types: custom;
Name: "VST"; Description: "VST"; Types: custom;
Name: "VST3"; Description: "VST3"; Types: custom;
Name: "CLAP"; Description: "CLAP"; Types: custom;

[Files]
Source: "{#PWD}\Builds\VisualStudio2022\x64\Release\Standalone Plugin\ShowMIDI.exe"; DestDir: "{app}"; Components: "Standalone"; Flags: ignoreversion
Source: "{#PWD}\Builds\VisualStudio2022\x64\Release\VST\ShowMIDI.dll"; DestDir: "{#VSTFolder}"; Components: "VST"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#PWD}\Builds\VisualStudio2022\x64\Release\VST3\ShowMIDI.vst3"; DestDir: "{#VST3Folder}"; Components: "VST3"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#PWD}\Builds\VisualStudio2022\x64\clap\ShowMIDI_artefacts\Release\ShowMIDI.clap"; DestDir: "{#CLAPFolder}"; Components: "CLAP"; Flags: ignoreversion recursesubdirs createallsubdirs

[Icons]
Name: "{group}\ShowMIDI"; Filename: "{app}\ShowMIDI.exe"; WorkingDir: "{app}"

; NOTE: Don't use "Flags: ignoreversion" on any shared system files

[Code]
var
  OkToCopyLog : Boolean;

procedure CurStepChanged(CurStep: TSetupStep);
begin
  if CurStep = ssDone then
    OkToCopyLog := True;
end;

procedure DeinitializeSetup();
begin
  if OkToCopyLog then
    FileCopy (ExpandConstant ('{log}'), ExpandConstant ('{app}\InstallationLogFile.log'), FALSE);
  RestartReplace (ExpandConstant ('{log}'), '');
end;

[UninstallDelete]
Type: files; Name: "{app}\InstallationLogFile.log"
