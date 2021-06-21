@echo off
cd /D %~dp0
REM font2bitmap -i %1 -o "%~d1%~p1%~n1" -m s -s 60 -c charset.utf8 -p 5
font2bitmap -i %1 -o "%~d1%~p1%~n1" -s 20 -c charset.utf8 -p 6 -t 0.33333
font2bitmap -i %1 -o "%~d1%~p1%~n1" -s 40 -c charset.utf8 -p 6 -t 0.66666
font2bitmap -i %1 -o "%~d1%~p1%~n1" -s 60 -c charset.utf8 -p 6 -t 1.0
font2bitmap -i %1 -o "%~d1%~p1%~n1" -s 80 -c charset.utf8 -p 6 -t 1.33333
font2bitmap -i %1 -o "%~d1%~p1%~n1" -s 100 -c charset.utf8 -p 6 -t 1.66666
REM font2bitmap -i %1 -o "%~d1%~p1%~n1" -m mixed -s 60 -c charset.utf8 -p 5
pause