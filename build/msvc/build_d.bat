@echo off
set random_val=%random%

set compile_flags=-MTd -nologo -Gm- -GR- -EHa- -Od -Oi -WX -W3 -wd4201 -wd4100 -wd4189 -DZT_DLL -FC -Z7 -LD -I"E:\Development\Projects\ZeroTolerance"
set link_flags=-incremental:no -opt:ref -PDB:..\..\bin\%1\%random_val%.hotload.zt_moco.pdb -OUT:..\..\bin\%1\%random_val%.hotload.zt_moco.dll user32.lib gdi32.lib winmm.lib shell32.lib

call "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat" 
cl %compile_flags% ..\..\src\unity_dll.cpp -Fmzt_moco.map /link %link_flags%
