// -*- C++ -*- print.h - printing stuff
// Copyright (C) 2001 Martin Bright <M.Bright@dpmms.cam.ac.uk>

// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

// $Id$

#ifndef RINGING_PRINT_H
#define RINGING_PRINT_H

#ifdef __GNUG__
#pragma interface
#endif

#include <ringing/common.h>
#if RINGING_OLD_INCLUDES
#include <iostream.h>
#include <map.h>
#else
#include <iostream>
#include <map>
#endif
#include <string>
#include <ringing/dimension.h>
#include <ringing/row.h>

RINGING_START_NAMESPACE


RINGING_USING_STD

class printpage;

struct text_style {
  string font;
  int size;

  enum alignment { left, right, centre };
};

class printrow {
public:
  // options : Options for outputting blue line
  // These together form a blue line `style'
  struct options {
    enum o_flags {
      numbers = 0x01,		// Print rows
      miss_numbers = 0x02       // Miss out numbers underneath lines
    };
    unsigned int flags;		// Flags as above
    text_style style;           // Text style
    dimension xspace, yspace;	// Spacing between places and rows

    struct line_style {
      dimension width;
    };
    map<int, line_style> lines; // What lines draw in what styles

    void defaults();
  };

  // This is the class from which all implementations are derived
  class base {
  public:
    virtual ~base() {}
    virtual void print(const row& r) = 0; // Print a row
    virtual void rule() = 0;
    virtual void set_position(const dimension& x, const dimension& y) = 0;
    virtual void new_column(const dimension& gap) = 0;
    virtual void set_options(const options& o) = 0;
    virtual const options& get_options() = 0;
    virtual void dot(int i) = 0;
    virtual void placebell(int i) = 0;
    virtual void text(const string& t, const dimension& x, 
		      text_style::alignment al, bool between, bool right) = 0;
  };

private:
  base* pr;

  // Stop people calling these
  printrow(const printrow&);
  printrow& operator=(const printrow&);

public:
  printrow(printpage& pp);
  printrow(printpage& pp, const options& o);
  ~printrow() { delete pr; }

  printrow& operator<<(const row& r) { pr->print(r); return *this; }
  void rule() { pr->rule(); }
  void set_position(const dimension& x, const dimension& y)  
    { pr->set_position(x, y); }
  void new_column(const dimension& gap)
    { pr->new_column(gap); }
  void set_options(const options& o)
    { pr->set_options(o); }
  const options& get_options()
    { return pr->get_options(); } 
  void dot(int i)
    { pr->dot(i); }
  void placebell(int i)
    { pr->placebell(i); } 
  void text(const string& t, const dimension& x, 
	    text_style::alignment al, bool between, bool right)
    { pr->text(t, x, al, between, right); }
};

class printpage {
public:
  virtual ~printpage() {}
  virtual void text(const string t, const dimension& x, const dimension& y, 
	       text_style::alignment al, const text_style& s) = 0;
  virtual void new_page() = 0;
protected:
  friend class printrow;
  virtual printrow::base* new_printrow(const printrow::options&) = 0;
};

RINGING_END_NAMESPACE

#endif
