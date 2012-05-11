# This file contains a buffered subclass of Xosd with bidirectional scrolling.
# Author::    Matt Boeh <mboeh@desperance.net>
# Copyright:: Copyright (c) 2004 Matt Boeh
# License::   MIT License, see COPYING
require 'xosd'

module XOSD
  
  # This class is an Xosd that automatically scrolls down as new strings are
  # added; the buffer can be cleared and scrolled in either direction.
  class XosdBuffer < Xosd
    # The list of lines in the buffer.
    attr_reader :buffer
    # The current top of the displayed portion of the buffer.
    attr_accessor :location
    # Flags whether each #location= and #puts action should run #show to update.
    attr_accessor :auto_update
    
    def initialize
      @buffer = []
      @location = 0
    end

    # Delete all lines from the buffer, clear the OSD, and scroll to the top. 
    def clear
      @buffer = []
      @location = 0
      scroll(lines)
    end
  
    # In addition to the regular Xosd work, also updates the current scrolled
    # display.
    def show
      scroll(lines)
      (0..(lines - 1)).each do |l|
        if @buffer[@location + l] then
          display_message(l, @buffer[@location + l])
        end
      end
      super
    end

    # Sets the top of the current scroll display.
    def location=(v)
      @location = v
      update if @auto_update
    end

    # Adds a string to the buffer. If the buffer is longer than the display,
    # scroll down.
    def puts(str)
      @buffer << str
      if @buffer.length > lines then
        @location += 1
      end
      update if @auto_update
    end
    
    alias update show
    alias << puts
  end

end
