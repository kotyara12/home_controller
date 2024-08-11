set project_dir=C:\Projects\PlatformIO\home_controller

cd %project_dir%
del /Q %project_dir%\ota\ota_upload.log
del /Q %project_dir%\ota\firmware\firmware_*.elf

git reflog expire --all --expire=now
git gc --prune=now --aggressive

pause
