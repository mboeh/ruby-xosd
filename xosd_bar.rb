# This file contains an Xosd with a movable bar and title.
# Author::    Matt Boeh <mboeh@desperance.net>
# Copyright:: Copyright (c) 2004 Matt Boeh
# License::   MIT License, see COPYING

require 'xosd'

module XOSD

  # This class is an Xosd with a nice bar.
  class XosdBar < Xosd
    # The title of this bar.
    attr_accessor :title
    # The value of this bar, from 1 to 100.
    attr_accessor :value
    # If true, the title is on top; else on the bottom.
    attr_accessor :title_on_top
    
    def XosdBar.new
      super(2)
    end

    def initialize(title = nil, value = nil)
      self.title = title
      self.value = value
      self.title_on_top = true
    end
    
    def show
      if title_on_top then
        if title then
          display_message(0, title)
        end
        if value then
          display_bar(1, value)
        end
      else
        if value then
          display_bar(0, value)
        end
        if title then
          display_message(1, title)
        end
      end
    end

    def value=(v)
      @value = v
      show
    end

    def title=(t)
      @title =t
      show
    end

    def title_on_top=(v)
      @title_on_top = v
      show
    end
  end

end
