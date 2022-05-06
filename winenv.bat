cmake -G "Visual Studio 16 2019" -B "..\Playground.build" .
if %errorlevel% NEQ 0 pause  

cmake --open "..\Playground.build"
if %errorlevel% NEQ 0 pause  