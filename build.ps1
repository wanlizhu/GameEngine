#New-Item -type Directory build
Push-Location build
#Remove-Item * -Recurse -Force

cmake -G "Visual Studio 15 2017 Win64" -Thost=x64 ..
cmake --build . --config debug
Pop-Location