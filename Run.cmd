@echo off
setlocal
cd /d "%~dp0"
if exist ".tools\Qt\6.8.3\msvc2022_64\bin" (
    set "PATH=%~dp0.tools\Qt\6.8.3\msvc2022_64\bin;%PATH%"
    set "QT_PLUGIN_PATH=%~dp0.tools\Qt\6.8.3\msvc2022_64\plugins"
)
if not exist "build\release\ObjectTreeDemo.exe" (
    echo Build ObjectTreeDemo.pro in Qt Creator first.
    echo This launcher expects build\release\ObjectTreeDemo.exe.
    pause
    exit /b 1
)
"build\release\ObjectTreeDemo.exe"
endlocal
