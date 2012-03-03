;Copyright (c) 2012 Valentin Galea
;
;Permission is hereby granted, free of charge, to any person obtaining a copy
;of this software and associated documentation files (the "Software"), to deal
;in the Software without restriction, including without limitation the rights
;to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
;copies of the Software, and to permit persons to whom the Software is
;furnished to do so, subject to the following conditions:
;
;The above copyright notice and this permission notice shall be included in
;all copies or substantial portions of the Software.
;
;THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
;IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
;FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
;AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
;LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
;OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
;THE SOFTWARE.

#define AppName "NewPas"
#define AppExeName "NewPas"
#define Editor "NewPasEdit"
#define AppVer "1.0"
#define OutputDir ".\"
#define AppUID "{5E6A5F0E-DAF7-4C1E-9C9A-FA3F29D725CA}"

[Setup]
AppId={{#AppUID}
AppName={#AppName}
AppVerName={#AppName}
AppPublisher=Valentin Galea
AppPublisherURL=https://www.facebook.com/valentin.galea
DefaultDirName={sd}\{#AppName}
DefaultGroupName={#AppName}
OutputDir={#OutputDir}
OutputBaseFilename={#AppExeName}-{#AppVer}
Compression=lzma
SolidCompression=yes
PrivilegesRequired=none
UninstallLogMode=append

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Files]
Source: "..\bin\{#AppExeName}.exe"; DestDir: "{app}\Compiler\bin"; Flags: ignoreversion
Source: "..\bin\{#Editor}.exe"; DestDir: "{app}\Compiler\bin"; Flags: ignoreversion
Source: "..\bin\SciLexer.dll"; DestDir: "{app}\Compiler\bin"; Flags: ignoreversion
Source: "..\bin\samples\*.pas"; DestDir: "{app}\Compiler\bin\samples\";

Source: "..\..\CarPhysics\bin\Car.exe"; DestDir: "{app}\CarPhysics\bin\"; Flags: ignoreversion
Source: "..\..\CarPhysics\bin\data\*.*"; DestDir: "{app}\CarPhysics\bin\data\";
Source: "..\..\CarPhysics\scripts\*.*"; DestDir: "{app}\CarPhysics\bin\scripts\";

[Icons]
Name: "{group}\{#AppName} Editor"; Filename: "{app}\Compiler\bin\{#Editor}.exe"; Parameters: """{app}\CarPhysics\bin\scripts\game.pas""";
Name: "{group}\{#AppName} Game"; Filename: "{app}\CarPhysics\bin\car.exe"; WorkingDir: "{app}\CarPhysics\bin\";
Name: "{group}\{cm:UninstallProgram,{#AppName}}"; Filename: "{uninstallexe}"

[Run]
Description: "{cm:LaunchProgram,{#AppName} Editor}"; Filename: "{app}\Compiler\bin\{#Editor}.exe"; Parameters: """{app}\CarPhysics\bin\scripts\game.pas"""; Flags: nowait postinstall skipifsilent;
Description: "{cm:LaunchProgram,{#AppName} Game}"; Filename: "{app}\CarPhysics\bin\car.exe"; WorkingDir: "{app}\CarPhysics\bin\"; Flags: nowait postinstall skipifsilent;

[Registry]
Root: HKCU; Subkey: "Software\NewPas\{#Editor}"; Flags: uninsdeletekey;
Root: HKCU; Subkey: "Software\NewPas"; Flags: uninsdeletekeyifempty;



