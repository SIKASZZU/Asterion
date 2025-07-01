@echo off
setlocal EnableDelayedExpansion

rem 1. Capture start time: convert HH:MM:SS.CC into centiseconds
for /F "tokens=1-4 delims=:.," %%a in ("%TIME%") do (
  set /A "start = (((%%a*60)+1%%b %% 100)*60 + 1%%c %% 100)*100 + 1%%d %% 100"
)

rem 2. Run your compile command (or any other)
%*

rem 3. Capture end time
for /F "tokens=1-4 delims=:.," %%a in ("%TIME%") do (
  set /A "end = (((%%a*60)+1%%b %% 100)*60 + 1%%c %% 100)*100 + 1%%d %% 100"
)

rem 4. Calculate elapsed centiseconds
set /A elapsed = end - start

rem 5. Break down into HH:MM:SS.CC
set /A hh = elapsed / (60*60*100)
set /A rest = elapsed %% (60*60*100)
set /A mm = rest / (60*100)
set /A rest = rest %% (60*100)
set /A ss = rest / 100
set /A cc = rest %% 100

rem 6. Pad with zeros
if %hh% lss 10 set hh=0%hh%
if %mm% lss 10 set mm=0%mm%
if %ss% lss 10 set ss=0%ss%
if %cc% lss 10 set cc=0%cc%

rem 7. Output results
echo Start Time: %TIME%
echo End Time:   %TIME%
echo Elapsed:    %hh%:%mm%:%ss%.%cc%