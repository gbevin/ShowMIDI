param (
    [Parameter(Mandatory=$true)][string]$version
 )

$curDir = Get-Location

Write-Output "Setting up development environment with Visuals Studio 2022"
Import-Module "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\Microsoft.VisualStudio.DevShell.dll"
Enter-VsDevShell -VsInstallPath "C:\Program Files\Microsoft Visual Studio\2022\Community\"

$Env:RELEASE_VERSION = "$version"
$Env:PATH_TO_JUCE = "$curDir\JUCE"

Write-Output "Changing location to $curDir"
Set-Location $curDir

$signId = "Open Source Developer, Geert Bevin"
$buildLocation = "Builds\VisualStudio2022\x64"

Write-Output "Deleting previous build from $buildLocation"
Remove-Item -LiteralPath $buildLocation -Force -Recurse

Write-Output "Building project"
MSBuild.exe .\Builds\VisualStudio2022\showmidi.sln /p:Configuration=Release /p:PreferredToolArchitecture=x64 /p:Platform=x64 /clp:ErrorsOnly

Write-Output "Building CLAP plugin"
cmake -B"$buildLocation\clap" -DCMAKE_BUILD_TYPE=Release
cmake --build "$buildLocation\clap" --config Release

Write-Output "Codesigning all artifacts"

& signtool sign /n "$signId" /t http://time.certum.pl/ /fd sha1 /v "$buildLocation\Release\Standalone Plugin\ShowMIDI.exe"
& signtool sign /n "$signId" /t http://time.certum.pl/ /fd sha1 /v "$buildLocation\Release\VST\ShowMIDI.dll"
& signtool sign /n "$signId" /t http://time.certum.pl/ /fd sha1 /v "$buildLocation\Release\VST3\ShowMIDI.vst3\Contents\x86_64-win\ShowMIDI.vst3"
& signtool sign /n "$signId" /t http://time.certum.pl/ /fd sha1 /v "$buildLocation\Release\LV2 Plugin\ShowMIDI.lv2\ShowMIDI.dll"
& signtool sign /n "$signId" /t http://time.certum.pl/ /fd sha1 /v "$buildLocation\clap\ShowMIDI_artefacts\Release\ShowMIDI.clap"

Write-Output "Building installer"
& 'C:\Program Files (x86)\Inno Setup 6\ISCC.exe' /DPWD="$curDir" /DPATH=./ /DVERSION="$Env:RELEASE_VERSION" Installers\ShowMIDI.iss

Write-Output "Code signing installer"

& signtool sign /n "$signId" /t http://time.certum.pl/ /fd sha1 /v "$curDir\Installers\ShowMIDI-Windows-$Env:RELEASE_VERSION.exe"