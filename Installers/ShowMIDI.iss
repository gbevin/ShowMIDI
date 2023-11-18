; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!
#define VERSION "0.7.0"
#define PWD "C:\Users\gbevin\source\repos\showmidi"
#define MyAppName "ShowMIDI"
#define MyAppPublisher "Uwyn"
#define MyAppURL "https://www.uwyn.com"
#define VSTFolder "{commoncf64}\VST\"
#define VST3Folder "{commoncf64}\VST3\"
#define CLAPFolder "{commoncf64}\CLAP\"
#define LV2Folder "{commoncf64}\LV2\"

[Setup]
; NOTE: The value of AppId uniquely identifies this application. Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{CF5E95CF-E92F-4C46-913D-A78713153914}
AppName={#MyAppName}
AppVersion={#VERSION}
AppVerName={#MyAppName}
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
Name: "LV2"; Description: "LV2"; Types: custom;


[Code]
var CPage: TInputDirWizardPage;

procedure InitializeWizard;
begin
  CPage := CreateInputDirPage(wpSelectComponents,
    'Confirm Plugin Directories', 'Where should the plugins be installed?',
    'To continue, click Next. ' +
      'If you would like to select a different folder, click Browse.',
    False, 'New Folder');

  CPage.Add('VST');
  CPage.Add('VST3');
  CPage.Add('CLAP');
  CPage.Add('LV2');

  CPage.Values[0] := ExpandConstant('{reg:HKLM\SOFTWARE\VST,VSTPluginsPath|{#VSTFolder}}');
  CPage.Values[1] := ExpandConstant('{#VST3Folder}');
  CPage.Values[2] := ExpandConstant('{#CLAPFolder}');
  CPage.Values[3] := ExpandConstant('{#LV2Folder}');
end;

procedure CurPageChanged(PageID: Integer);
begin
  if PageID = CPage.ID then
  begin
    CPage.PromptLabels[0].Enabled := WizardIsComponentSelected('VST');
    CPage.Edits[0].Enabled := CPage.PromptLabels[0].Enabled;
    CPage.Buttons[0].Enabled := CPage.PromptLabels[0].Enabled;

    CPage.PromptLabels[1].Enabled := WizardIsComponentSelected('VST3');
    CPage.Edits[1].Enabled := CPage.PromptLabels[1].Enabled;
    CPage.Buttons[1].Enabled := CPage.PromptLabels[1].Enabled;

    CPage.PromptLabels[2].Enabled := WizardIsComponentSelected('CLAP');
    CPage.Edits[2].Enabled := CPage.PromptLabels[2].Enabled;
    CPage.Buttons[2].Enabled := CPage.PromptLabels[2].Enabled;

    CPage.PromptLabels[3].Enabled := WizardIsComponentSelected('LV2');
    CPage.Edits[3].Enabled := CPage.PromptLabels[3].Enabled;
    CPage.Buttons[3].Enabled := CPage.PromptLabels[3].Enabled;
  end;
end;

function GetVstDir(Param: String): String;
begin
  Result := CPage.Values[0];
end;

function GetVst3Dir(Param: String): String;
begin
  Result := CPage.Values[1];
end;

function GetClapDir(Param: String): String;
begin
  Result := CPage.Values[2];
end;

function GetLv2Dir(Param: String): String;
begin
  Result := CPage.Values[3];
end;

[Files]
Source: "{#PWD}\Builds\VisualStudio2022\x64\Release\Standalone Plugin\ShowMIDI.exe"; DestDir: "{app}"; Components: "Standalone"; Flags: ignoreversion
Source: "{#PWD}\Builds\VisualStudio2022\x64\Release\VST\ShowMIDI.dll"; DestDir: "{code:GetVstDir}"; Components: "VST"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#PWD}\Builds\VisualStudio2022\x64\Release\VST3\ShowMIDI.vst3\*"; DestDir: "{code:GetVst3Dir}\ShowMIDI.vst3"; Components: "VST3"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#PWD}\Builds\VisualStudio2022\x64\clap\ShowMIDI_artefacts\Release\ShowMIDI.clap"; DestDir: "{code:GetClapDir}"; Components: "CLAP"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#PWD}\Builds\VisualStudio2022\x64\Release\LV2 Plugin\ShowMIDI.lv2\*"; DestDir: "{code:GetLv2Dir}\ShowMIDI.lv2"; Components: "LV2"; Flags: ignoreversion recursesubdirs createallsubdirs

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
