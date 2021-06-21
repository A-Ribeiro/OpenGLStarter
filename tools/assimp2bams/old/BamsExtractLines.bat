@echo off
%~d0
cd %~dp0
assimp2bams %1 "%~d1%~p1%~n1.bams"
pause