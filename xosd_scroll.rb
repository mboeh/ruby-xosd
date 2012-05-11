# This file contains a scrolling Xosd that stores no past data.
# Author::    Matt Boeh <mboeh@desperance.net>
# Copyright:: Copyright (c) 2004 Matt Boeh
# License::   MIT License, see COPYING

require 'xosd'

module XOSD

  # This class is an Xosd that automatically scrolls down, not up.
  class XosdScroll < Xosd
    # The current position in the XOSD box.
    attr_accessor :location
    
    def initialize
      @location = 0
      @last_time = Time.now.to_i
    end

    # Clear the current display and reset the location.
    def clear
      @location = 0
      scroll(lines)
    end
  
    # Adds a line at the current location, scrolling if necessary.
    def puts(str)
      if Time.now.to_i - @last_time >= @timeout then
        clear
      end
      if @location == lines then
        @location = lines - 1
        scroll(1)
      end
      display_message(@location, str)
      @last_time = Time.now.to_i
      @location += 1
    end
    
    alias << puts
  end

end
