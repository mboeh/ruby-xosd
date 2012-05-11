
require 'test/unit'
require 'xosd'
require 'thread'

include XOSD

class XosdTest < Test::Unit::TestCase
  def setup
    @osd = Xosd::new(3)
  end

  def teardown
    begin
      @osd.hide
    rescue

    end
  end

  def test_message
    assert_nil(@osd.display_message(0, "ricola"))
    assert_raise(XosdError) do
      @osd.display_message(4, "aroooo")
    end
  end 

  def test_slider
    assert_nil(@osd.display_slider(0, 50))
    assert_nil(@osd.display_slider(0, 1000303))
    assert_nil(@osd.display_slider(0, -13045))
    assert_raise(XosdError) do
      @osd.display_slider(3, 43)
    end
  end 
  
  def test_bar
    assert_nil(@osd.display_bar(0, 50))
    assert_nil(@osd.display_bar(0, 1000303))
    assert_nil(@osd.display_bar(0, -13045))
    assert_raise(XosdError) do
      @osd.display_bar(3, 43)
    end
  end 

  def test_onscreen_p
    assert(!@osd.onscreen?)
    @osd.display_message(0, "hoooof!")
    assert(@osd.onscreen?)
  end
 
  def test_timeout
    assert_raise(TypeError) do
      @osd.timeout = "furp"
    end
    assert_nothing_raised do
      @osd.timeout = nil
      @osd.timeout = -1
      @osd.timeout = 2 
    end
    assert_equal(2, @osd.timeout)
    @osd.display_message(0, "foooop")
    assert(@osd.onscreen?)
  end

  def test_wait
    @osd.timeout = 5
    # I hate waiting :-p
    @osd.wait(1) if $DEBUG
    assert_equal(5, @osd.timeout)
    assert(!@osd.onscreen?)
  end

  def test_hide_show
    @osd.display_message(0, "chonk")
    assert(@osd.onscreen?)
    @osd.hide
    assert(!@osd.onscreen?)
    @osd.show
    assert(@osd.onscreen?)
  end

  def test_positioning
    # Here's one where we're going to have to depend on the tester to watch and see if all is well.
    @osd.display_message(0, "TESTERY")
    @osd.display_slider(1, 50)
    @osd.display_bar(2, 77)

    @osd.position = XOSD::TOP
    @osd.align = XOSD::LEFT
    assert_equal(XOSD::TOP, @osd.position)
    sleep 1 if $DEBUG
    @osd.align = XOSD::CENTER
    sleep 1 if $DEBUG
    @osd.align = XOSD::RIGHT
    sleep 1 if $DEBUG

    @osd.align = XOSD::LEFT
    @osd.position = XOSD::MIDDLE
    assert_equal(XOSD::MIDDLE, @osd.position)
    sleep 1 if $DEBUG
    @osd.align = XOSD::CENTER
    sleep 1 if $DEBUG
    @osd.align = XOSD::RIGHT
    sleep 1 if $DEBUG
    
    @osd.align = XOSD::LEFT
    @osd.position = XOSD::BOTTOM
    assert_equal(XOSD::BOTTOM, @osd.position)
    sleep 1 if $DEBUG
    @osd.align = XOSD::CENTER
    sleep 1 if $DEBUG
    @osd.align = XOSD::RIGHT
    sleep 1 if $DEBUG

  end
 
  def test_offsets
    @osd.display_message(0, "JACKIE GLEASON")
    (1..300).step(10) do |oset|
      sleep 0.1 if $DEBUG
      @osd.vertical_offset = oset
      @osd.show
    end

    (1..300).step(10) do |oset|
      sleep 0.1 if $DEBUG
      @osd.horizontal_offset = oset
      @osd.show
    end

    @osd.position = XOSD::MIDDLE
    @osd.align = XOSD::CENTER

    assert_equal(0, @osd.vertical_offset)
    assert_equal(0, @osd.horizontal_offset)
  end

  def test_threads
    oosd = Xosd.new(3)
    @osd.display_message(0, "STRAIGHT TO THE MOON")
    @osd.position = XOSD::MIDDLE
    oosd.align = XOSD::RIGHT
    oosd.position = XOSD::MIDDLE
    oosd.display_message(2, "PHILIP J. FRY")

    t1 = Thread.new do
      @osd.wait(5)
    end
    
    t2 = Thread.new do
      oosd.wait(3)
    end
  end
end
