@echo off

for %%f in (Source/*.cpp Source/*.h) do (

	clang-format.exe --style=file -i Source/%%f
)