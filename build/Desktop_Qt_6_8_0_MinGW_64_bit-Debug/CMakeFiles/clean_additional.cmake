# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "3rdparty\\qxlsx\\QXlsx\\CMakeFiles\\QXlsx_autogen.dir\\AutogenUsed.txt"
  "3rdparty\\qxlsx\\QXlsx\\CMakeFiles\\QXlsx_autogen.dir\\ParseCache.txt"
  "3rdparty\\qxlsx\\QXlsx\\QXlsx_autogen"
  "CMakeFiles\\PriceTagMaster_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\PriceTagMaster_autogen.dir\\ParseCache.txt"
  "PriceTagMaster_autogen"
  )
endif()
