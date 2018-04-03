; -- PaleoDataView.iss --
; This will install the compiled PaleoDataView exe, all its Resources, and the Source

[Setup]
AppName=PaleoDataView
AppVersion=0.7
DefaultDirName={pf}\PaleoDataView
DefaultGroupName=PaleoDataView
UninstallDisplayIcon={app}\PaleoDataView.exe
Compression=lzma2
SolidCompression=yes
OutputDir=userdocs:Inno Setup Examples Output
UninstallDisplayName=PaleoDataView
AppPublisher=PalMod
AppPublisherURL=http://www.palmod.de/
LicenseFile=license.txt
InfoBeforeFile=ReadMe.txt
[Components]
Name: "main"; Description: "Minimum Installation"; Types: full compact custom; Flags: fixed
Name: "data"; Description: "Data Library"; Types: full custom
Name: "source"; Description: "Source Files"; Types: full custom

[Dirs]
Name: "{userdocs}\PaleoDataView"; Flags: uninsneveruninstall; Permissions: everyone-modify
Name: "{userdocs}\PaleoDataView\Default\Isotopes";  Flags: uninsneveruninstall; Permissions: everyone-modify
Name: "{userdocs}\PaleoDataView\Default\Import";  Flags: uninsneveruninstall; Permissions: everyone-modify
Name: "{userdocs}\PaleoDataView\Default\Age";  Flags: uninsneveruninstall; Permissions: everyone-modify
Name: "{userdocs}\PaleoDataView\Resources\Plot";  Flags: uninsneveruninstall; Permissions: everyone-modify
Name: "{userdocs}\PaleoDataView\Default\Derived Data";  Flags: uninsneveruninstall; Permissions: everyone-modify
Name: "{userdocs}"; Flags: uninsneveruninstall; Permissions: everyone-modify

[Files]
; -- Install Minimum Files --
; PaleoView.exe
Source: "PaleoDataView.exe"; DestDir: "{app}"; Components: main
; Libraries: NetCDF
Source: "hdf5.dll"; DestDir: "{app}"; Components: main
Source: "hdf5_hl.dll"; DestDir: "{app}"; Components: main
Source: "netcdf.dll"; DestDir: "{app}"; Components: main
Source: "zlib1.dll"; DestDir: "{app}"; Components: main
Source: "msvcr100.dll"; DestDir: "{app}"; Components: main
; Libraries: Qt  
Source: "Qt5Cored.dll"; DestDir:"{app}"; Components: main
Source: "Qt5Guid.dll"; DestDir:"{app}"; Components: main
Source: "Qt5Widgetsd.dll"; DestDir:"{app}"; Components: main
Source: "Qt5Svgd.dll"; DestDir:"{app}"; Components: main
; Liraries: MinGw
Source: "libstdc++-6.dll"; DestDir:"{app}"; Components: main
Source: "libgcc_s_dw2-1.dll"; DestDir:"{app}"; Components: main
Source: "libwinpthread-1.dll"; DestDir:"{app}"; Components: main
; Windows
Source: "D3Dcompiler_47.dll"; DestDir:"{app}"; Components: main
Source: "libEGLd.dll"; DestDir:"{app}"; Components: main
Source: "libGLESV2d.dll"; DestDir:"{app}"; Components: main
Source: "opengl32sw.dll"; DestDir:"{app}"; Components: main
; Plugins
Source: "iconengines\*.*"; DestDir:"{app}\iconengines"; Components: main
Source: "imageformats\*.*"; DestDir:"{app}\imageformats"; Components: main
Source: "platforms\*.*"; DestDir:"{app}\platforms"; Components: main
Source: "translations\*.*"; DestDir:"{app}\translations"; Components: main
; Create Minimum Resource Data
Source: "PaleoDataView\Resources\Map\*.*"; DestDir:"{userdocs}\PaleoDataView\Resources\Map"; Components: main
Source: "PaleoDataView\Resources\Hydro\*.*"; DestDir:"{userdocs}\PaleoDataView\Resources\Hydro"; Components: main
Source: "PaleoDataView\Resources\Plot\*.*"; DestDir:"{userdocs}\PaleoDataView\Resources\Plot"; Components: main
Source: "PaleoDataView\Resources\Calibration\*.*"; DestDir:"{userdocs}\PaleoDataView\Resources\Calibration"; Components: main
Source: "PaleoDataView\Resources\Icons\*.*"; DestDir:"{userdocs}\PaleoDataView\Resources\Icons"; Components: main
Source: "PaleoDataView\Resources\Bacon\*.*"; DestDir:"{userdocs}\PaleoDataView\Resources\Bacon"; Components: main; Flags: recursesubdirs
Source: "PDV_icon.ico"; DestDir:"{app}"; Components: main


; -- Install Data --
; Isotope Data
Source: "PaleoDataView\Default\Isotopes\*.nc"; DestDir:"{userdocs}\PaleoDataView\Default\Isotopes"; Components: data;  Flags: uninsneveruninstall 
Source: "PaleoDataView\Default\Import\*.*"; DestDir:"{userdocs}\PaleoDataView\Default\Import"; Components: data;  Flags: uninsneveruninstall 
; Age Models
Source: "PaleoDataView\Default\Age\*.age"; DestDir:"{userdocs}\PaleoDataView\Default\Age"; Components: data;  Flags: uninsneveruninstall 

; -- Install Source --
Source: "source\*.*";DestDir:"{app}\source"; Components: source; Flags: recursesubdirs


[Icons]
Name: "{commondesktop}\PaleoDataView"; Filename: "{app}\PaleoDataView.exe"; WorkingDir: "{userdocs}"; IconFilename: "{app}\PDV_icon.ico"
Name: "{commonprograms}\PaleoDataView"; Filename: "{app}\PaleoDataView.exe"; WorkingDir: "{userdocs}"; IconFilename: "{app}\PDV_icon.ico"

