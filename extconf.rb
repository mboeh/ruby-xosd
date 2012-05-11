require 'mkmf'

if find_library("X11", "XOpenDisplay", "/usr/X11/lib", "/usr/X11R6/lib", "/usr/openwin/lib") and 
   find_library("Xt", "") and
   find_library("Xext", "") and
   find_library("xosd", "xosd_create") then
  dir_config('xosd')
  create_makefile('xosd')
else
  puts "Couldn't find X11"
end
