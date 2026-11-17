#!/bin/bash

version="$1"
sed -i -E "s/^(set\(APP_VERSION \")[^\"]+(\")/\1$version\2/" CMakeLists.txt
sed -i -E "s/^(#define MyAppVersion \")[^\"]+(\")/\1$version\2/" resources/windows/Installer.iss