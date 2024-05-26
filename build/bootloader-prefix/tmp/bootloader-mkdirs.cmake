# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/michaelb/Projects/esp/esp-idf/components/bootloader/subproject"
  "/home/michaelb/Projects/green_house_firmware-main/build/bootloader"
  "/home/michaelb/Projects/green_house_firmware-main/build/bootloader-prefix"
  "/home/michaelb/Projects/green_house_firmware-main/build/bootloader-prefix/tmp"
  "/home/michaelb/Projects/green_house_firmware-main/build/bootloader-prefix/src/bootloader-stamp"
  "/home/michaelb/Projects/green_house_firmware-main/build/bootloader-prefix/src"
  "/home/michaelb/Projects/green_house_firmware-main/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/michaelb/Projects/green_house_firmware-main/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/michaelb/Projects/green_house_firmware-main/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
