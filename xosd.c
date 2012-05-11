#include <ruby.h>
#include <xosd.h>

VALUE mXOSD;
VALUE cXosd;
/*
 * An error signifying a problem in the xosd library. The message comes straight
 * from the library.
 */
VALUE ceXosdError;

/* Destroys the on-screen display and frees its memory. */
static void Xosd_free(void *p) {
  xosd_destroy(p);
}

/* Checks a libxosd return value to see if it was an error; if so, raises it.
 * Otherwise, returns the value. */
signed int chkerror(signed int retval) {
  if (retval == -1) {
    rb_raise(ceXosdError, xosd_error);
  }
  return retval;
}

/* A "safe" version of xosd_show that doesn't re-show the OSD if it's visible. */
void safe_show(xosd* osd) {
  if (!chkerror(xosd_is_onscreen(osd))) {
    chkerror(xosd_show(osd));
  }
}

/* A "safe" version of xosd_hide that doesn't re-hide the OSD if it's hidden. */
void safe_hide(xosd* osd) {
  if (chkerror(xosd_is_onscreen(osd))) {
    chkerror(xosd_hide(osd));
  }
}

/* Allocates and initializes a new OSD object. Passed the number of lines used
 * in the OSD buffer (defaults to 1).
 * The number of lines CANNOT be changed after this point!
 */
VALUE Xosd_new(int argc, VALUE* argv, VALUE class) {
  xosd* osd;
  VALUE xdata;
  int numlines;
  if (argc > 0) {
    numlines = NUM2INT(argv[0]);
  } else {
    numlines = 1;
  }

  osd = xosd_create(numlines);
  xdata = Data_Wrap_Struct(class, 0, Xosd_free, osd);
  rb_iv_set(xdata, "@font", rb_str_new2("fixed"));
  rb_iv_set(xdata, "@color", rb_str_new2("green"));
  rb_iv_set(xdata, "@outline_color", rb_str_new2("black"));
  rb_iv_set(xdata, "@shadow_color", rb_str_new2("black"));
  rb_iv_set(xdata, "@timeout", INT2FIX(-1));
  rb_iv_set(xdata, "@vertical_offset", INT2FIX(0));
  rb_iv_set(xdata, "@horizontal_offset", INT2FIX(0));
  rb_iv_set(xdata, "@shadow_offset", INT2FIX(0));
  rb_iv_set(xdata, "@outline_offset", INT2FIX(0));
  rb_iv_set(xdata, "@position", INT2FIX(XOSD_top));
  rb_iv_set(xdata, "@align", INT2FIX(XOSD_left));
  rb_iv_set(xdata, "@lines", INT2FIX(numlines));

  rb_obj_call_init(xdata, 0, NULL);

  return xdata;
}

/* Display a message at a given line. Throws an XosdError if the line requested
 * exceeds the number of lines in the buffer. */
VALUE Xosd_display_message(VALUE self, VALUE line, VALUE string) {
  xosd* osd;
  int to_line = NUM2INT(line);
  char* message = STR2CSTR(string);
  
  int retval;
  
  Data_Get_Struct(self, xosd, osd);

  retval = xosd_display(osd, to_line, XOSD_string, message);

  if (retval == -1) {
    rb_raise(ceXosdError, xosd_error);
  }
  return Qnil;
}
 
VALUE Xosd_display_slider(VALUE self, VALUE line, VALUE percent) {
  xosd* osd;
  int pcentage = NUM2INT(percent);
  int to_line = NUM2INT(line);
  int retval;
  
  if (pcentage > 100) {
    pcentage = 100;
  } else if (pcentage < 1) {
    pcentage = 1;
  }

  Data_Get_Struct(self, xosd, osd);

  retval = xosd_display(osd, to_line, XOSD_slider, pcentage);
  
  if (retval == -1) {
    rb_raise(ceXosdError, xosd_error);
  }
  return Qnil;
}

VALUE Xosd_display_bar(VALUE self, VALUE line, VALUE percent) {
  xosd* osd;
  int pcentage = NUM2INT(percent);
  int to_line = NUM2INT(line);
  int retval;
  
  if (pcentage > 100) {
    pcentage = 100;
  } else if (pcentage < 1) {
    pcentage = 1;
  }

  Data_Get_Struct(self, xosd, osd);

  retval = xosd_display(osd, to_line, XOSD_percentage, pcentage);
  
  if (retval == -1) {
    rb_raise(ceXosdError, xosd_error);
  }
  return Qnil;
}

VALUE Xosd_onscreen_p(VALUE self) {
  xosd* osd;
  int retval;
  
  Data_Get_Struct(self, xosd, osd);

  retval = xosd_is_onscreen(osd);

  if (retval == -1) {
    rb_raise(ceXosdError, xosd_error);
  } else if (retval) {
    return Qtrue;
  } else {
    return Qfalse;
  }
}   

VALUE Xosd_timeout_set(VALUE self, VALUE seconds) {
  int tmout;
  int retval;
  xosd* osd; Data_Get_Struct(self, xosd, osd);

  if (NIL_P(seconds)) {
    tmout = -1;
  } else if (FIXNUM_P(seconds)) {
    tmout = FIX2INT(seconds);
  } else {
    rb_raise(rb_eTypeError, "timeout must be an integer or nil!");
  }
  
  rb_iv_set(self, "@timeout", seconds);
  chkerror(xosd_set_timeout(osd, tmout));
  
  return seconds;
}

VALUE Xosd_timeout(VALUE self) {
  return rb_iv_get(self, "@timeout");
}

VALUE Xosd_wait(VALUE self, VALUE howlong) {
  VALUE slept = Qnil;
  xosd* osd; Data_Get_Struct(self, xosd, osd);

  chkerror(xosd_set_timeout(osd, -1));
  
  safe_show(osd);
  slept = rb_funcall(rb_mKernel, rb_intern("sleep"), 1, howlong);
  safe_hide(osd);
  chkerror(xosd_set_timeout(osd, INT2FIX(rb_iv_get(self, "@timeout"))));

  return slept;
}

VALUE Xosd_hide(VALUE self) {
  xosd* osd; Data_Get_Struct(self, xosd, osd);

  safe_hide(osd);

  return self;
}

VALUE Xosd_show(VALUE self) {
  xosd* osd; Data_Get_Struct(self, xosd, osd);

  safe_show(osd);
  return self;
}

VALUE Xosd_scroll(VALUE self, VALUE lines) {
  xosd* osd; Data_Get_Struct(self, xosd, osd);

  chkerror(xosd_scroll(osd, FIX2INT(lines)));
  return self;
}

VALUE Xosd_position_set(VALUE self, VALUE pos) {
  xosd* osd; Data_Get_Struct(self, xosd, osd);

  switch (NUM2INT(pos)) {
    case XOSD_top:
      rb_iv_set(self, "@position", pos);
      xosd_set_pos(osd, XOSD_top);
      break;
    case XOSD_bottom:
      rb_iv_set(self, "@position", pos);
      xosd_set_pos(osd, XOSD_bottom);
      break;
    case XOSD_middle:
      rb_iv_set(self, "@position", pos);
      /* xosd has unpredictable behavior with a voffset and XOSD_middle */
      rb_iv_set(self, "@vertical_offset", INT2FIX(0));
      xosd_set_vertical_offset(osd, 0);
      xosd_set_pos(osd, XOSD_middle);
      break;
    default:
      rb_raise(rb_eRangeError, "OSD position must be one of TOP, MIDDLE, or BOTTOM!");
  }
  
  return pos; 
}

VALUE Xosd_position(VALUE self) {   
  return rb_iv_get(self, "@position");
}

VALUE Xosd_align_set(VALUE self, VALUE pos) {
  xosd* osd; Data_Get_Struct(self, xosd, osd);

  switch (NUM2INT(pos)) {
    case XOSD_left:
      rb_iv_set(self, "@align", pos);
      xosd_set_align(osd, XOSD_left);
      break;
    case XOSD_right:
      rb_iv_set(self, "@align", pos);
      xosd_set_align(osd, XOSD_right);
      break;
    case XOSD_center:
      rb_iv_set(self, "@align", pos);
      /* xosd has unpredictable behavior with a voffset and XOSD_middle */
      rb_iv_set(self, "@horizontal_offset", INT2FIX(0));
      xosd_set_horizontal_offset(osd, 0);
      xosd_set_align(osd, XOSD_center);
      break;
    default:
      rb_raise(rb_eRangeError, "OSD align must be one of TOP, MIDDLE, or BOTTOM!");
  }
  
  return pos; 
}

VALUE Xosd_align(VALUE self) {   
  return rb_iv_get(self, "@align");
}

VALUE Xosd_font_set(VALUE self, VALUE font) {
  xosd* osd; Data_Get_Struct(self, xosd, osd);

  Check_Type(font, T_STRING);
  chkerror(xosd_set_font(osd, STR2CSTR(font)));
  rb_iv_set(self, "@font", font);
  return font;
}

VALUE Xosd_font(VALUE self) {
  return rb_iv_get(self, "@font");
}

#include <stdio.h>

VALUE Xosd_color_set(VALUE self, VALUE color) {
  xosd* osd; Data_Get_Struct(self, xosd, osd);

  Check_Type(color, T_STRING);
  chkerror(xosd_set_colour(osd, STR2CSTR(color)));
  rb_iv_set(self, "@color", color);
  return color;
}

VALUE Xosd_color(VALUE self) {
  return rb_iv_get(self, "@color");
}

VALUE Xosd_outline_color_set(VALUE self, VALUE outline_color) {
  xosd* osd; Data_Get_Struct(self, xosd, osd);

  Check_Type(outline_color, T_STRING);
  chkerror(xosd_set_outline_colour(osd, STR2CSTR(outline_color)));
  rb_iv_set(self, "@outline_color", outline_color);
  return outline_color;
}

VALUE Xosd_outline_color(VALUE self) {
  return rb_iv_get(self, "@outline_color");
}

VALUE Xosd_shadow_color_set(VALUE self, VALUE shadow_color) {
  xosd* osd; Data_Get_Struct(self, xosd, osd);

  Check_Type(shadow_color, T_STRING);
  chkerror(xosd_set_shadow_colour(osd, STR2CSTR(shadow_color)));
  rb_iv_set(self, "@shadow_color", shadow_color);
  return shadow_color;
}

VALUE Xosd_shadow_color(VALUE self) {
  return rb_iv_get(self, "@shadow_color");
}

VALUE Xosd_bar_length_set(VALUE self, VALUE bar_length) {
  xosd* osd; Data_Get_Struct(self, xosd, osd);

  Check_Type(bar_length, T_FIXNUM);
  chkerror(xosd_set_bar_length(osd, FIX2INT(bar_length)));
  rb_iv_set(self, "@bar_length", bar_length);
  return bar_length;
}

VALUE Xosd_bar_length(VALUE self) {
  return rb_iv_get(self, "@bar_length");
}

VALUE Xosd_lines(VALUE self) {
  return rb_iv_get(self, "@lines");
}

VALUE Xosd_vertical_offset_set(VALUE self, VALUE offset) {
  xosd* osd; Data_Get_Struct(self, xosd, osd);

  xosd_set_vertical_offset(osd, FIX2INT(offset));
  rb_iv_set(self, "@vertical_offset", offset);

  return offset;
}

VALUE Xosd_vertical_offset(VALUE self) {
  return rb_iv_get(self, "@vertical_offset");
}

VALUE Xosd_horizontal_offset_set(VALUE self, VALUE offset) {
  xosd* osd; Data_Get_Struct(self, xosd, osd);

  xosd_set_horizontal_offset(osd, FIX2INT(offset));
  rb_iv_set(self, "@horizontal_offset", offset);

  return offset;
}

VALUE Xosd_horizontal_offset(VALUE self) {
  return rb_iv_get(self, "@horizontal_offset");
}

VALUE Xosd_shadow_offset_set(VALUE self, VALUE offset) {
  xosd* osd; Data_Get_Struct(self, xosd, osd);

  xosd_set_shadow_offset(osd, FIX2INT(offset));
  rb_iv_set(self, "@shadow_offset", offset);

  return offset;
}

VALUE Xosd_shadow_offset(VALUE self) {
  return rb_iv_get(self, "@shadow_offset");
}

VALUE Xosd_outline_offset_set(VALUE self, VALUE offset) {
  xosd* osd; Data_Get_Struct(self, xosd, osd);

  xosd_set_outline_offset(osd, FIX2INT(offset));
  rb_iv_set(self, "@outline_offset", offset);

  return offset;
}

VALUE Xosd_outline_offset(VALUE self) {
  return rb_iv_get(self, "@outline_offset");
}

void Init_xosd() {
  mXOSD = rb_define_module("XOSD");
  cXosd = rb_define_class_under(mXOSD, "Xosd", rb_cObject);
  ceXosdError = rb_define_class_under(mXOSD, "XosdError", rb_eStandardError);
  
  rb_define_const(mXOSD, "TOP", INT2FIX(XOSD_top));
  rb_define_const(mXOSD, "MIDDLE", INT2FIX(XOSD_middle));
  rb_define_const(mXOSD, "BOTTOM", INT2FIX(XOSD_bottom));
  rb_define_const(mXOSD, "LEFT", INT2FIX(XOSD_left));
  rb_define_const(mXOSD, "CENTER", INT2FIX(XOSD_center));
  rb_define_const(mXOSD, "RIGHT", INT2FIX(XOSD_right));

  rb_define_singleton_method(cXosd, "new", Xosd_new, -1);
  rb_define_method(cXosd, "display_message", Xosd_display_message, 2);
  rb_define_method(cXosd, "display_slider", Xosd_display_slider, 2);
  rb_define_method(cXosd, "display_bar", Xosd_display_bar, 2);
  rb_define_method(cXosd, "onscreen?", Xosd_onscreen_p, 0);
  rb_define_method(cXosd, "timeout=", Xosd_timeout_set, 1);
  rb_define_method(cXosd, "timeout", Xosd_timeout, 0);
  rb_define_method(cXosd, "wait", Xosd_wait, 1);
  rb_define_method(cXosd, "scroll", Xosd_scroll, 1);
  rb_define_method(cXosd, "hide", Xosd_hide, 0);
  rb_define_method(cXosd, "show", Xosd_show, 0);
  rb_define_method(cXosd, "position=", Xosd_position_set, 1);
  rb_define_method(cXosd, "position", Xosd_position, 0);
  rb_define_method(cXosd, "align=", Xosd_align_set, 1);
  rb_define_method(cXosd, "align", Xosd_align, 0);

  rb_define_method(cXosd, "font=", Xosd_font_set, 1);
  rb_define_method(cXosd, "font", Xosd_font, 0);
  rb_define_method(cXosd, "color=", Xosd_color_set, 1);
  rb_define_method(cXosd, "color", Xosd_color, 0);
  rb_define_method(cXosd, "outline_color=", Xosd_outline_color_set, 1);
  rb_define_method(cXosd, "outline_color", Xosd_outline_color, 0);
  rb_define_method(cXosd, "shadow_color=", Xosd_shadow_color_set, 1);
  rb_define_method(cXosd, "shadow_color", Xosd_shadow_color, 0);

  rb_define_method(cXosd, "bar_length=", Xosd_bar_length_set, 1);
  rb_define_method(cXosd, "bar_length", Xosd_bar_length, 0);
  rb_define_method(cXosd, "lines", Xosd_lines, 0);
  rb_define_method(cXosd, "vertical_offset=", Xosd_vertical_offset_set, 1);
  rb_define_method(cXosd, "vertical_offset", Xosd_vertical_offset, 0);
  rb_define_method(cXosd, "horizontal_offset=", Xosd_horizontal_offset_set, 1);
  rb_define_method(cXosd, "horizontal_offset", Xosd_horizontal_offset, 0);
  rb_define_method(cXosd, "shadow_offset=", Xosd_shadow_offset_set, 1);
  rb_define_method(cXosd, "shadow_offset", Xosd_shadow_offset, 0);
  rb_define_method(cXosd, "outline_offset=", Xosd_outline_offset_set, 1);
  rb_define_method(cXosd, "outline_offset", Xosd_outline_offset, 0);


}
