$files=Get-ChildItem -Path Source | %{ $_.FullName }

clang-format.exe --style=file -i $files
