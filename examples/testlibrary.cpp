// -*- C++ -*- testbase.cpp - test the base classes
// Copyright (C) 2001 Mark Banner <mark@standard8.co.uk>

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

#if RINGING_OLD_INCLUDES
#include <iostream.h>
#include <stdexcept.h>
#else
#include <iostream>
#include <stdexcept>
#endif
#include <ringing/method.h>
#include <ringing/cclib.h>
#include <ringing/mslib.h>
#include <string>

#if RINGING_USE_NAMESPACES
using namespace ringing;
#endif

void print_row_block(const row_block& b)
{
  cout << "\t" << b[0] << " " 
       << ((b[0].sign() < 0) ? '-' : '+') << " " << endl;
  for(int i = 1;i < b.size();i++)
    cout << b.get_changes()[i-1] << "\t" << b[i]  << " " 
	 << ((b[i].sign() < 0) ? '-' : '+') << " " << endl;
}

int main()
{
  cout << "\nTesting method libraries...\n";

  mslib::registerlib();
  cclib::registerlib();

  {
    string filename;

    cout << "\nFirst enter a file that does not exist: ";
    getline(cin, filename);

    library l(filename.c_str());
    if (!l.good())
      {
	cout << "Good, you entered a file that does not exist\n";
      }
    else
      {
	cout << "Are you sure this isn't an existing method library file?\n";
      }
  }

  {
    string filename;
    
    cout << "\nNow enter a file that is not a method library: ";
    getline(cin, filename);

    library l(filename.c_str());
    if (!l.good())
      {
	cout << "Good, I have no valid libraries registered for that file\n";
      }
    else
      {
	cout << "If this is not a method library file, please email it to the authors as we have a bug.\n";
      }
  }

  {
    string filename;
    
    cout << "\nNow enter a valid method library file name: ";
    getline(cin, filename);

    library l(filename.c_str());
    if (!l.good())
      {
	cerr << "I cannot read that file.\n";
	return 1;
      }
    else
      {
	cout << "A library can read that file.\n";

	cout << "\nTesting multiple loads of methods...\n";
	string methname;
	cout << "\nPlease enter a method name: ";
	getline(cin, methname);

	try
	  {
	    method m(l.load(methname.c_str()));
	    
	    char s[80];
	    cout << m.fullname(s) << endl;
	    
	    print_row_block((row_block)m);
	    
	    cout << "This method is "
		 << ((m.isregular()) ? "" : "ir")
		 << "regular"
		 << ((m.isdouble()) ? " and double" : "")
		 << ".\n";
	    cout << "Lead head code: " << m.lhcode() << endl;
	  }
	catch (exception &e)
	  {
	    cerr << "Error cannot load method: " << e.what() << endl;
	  }

	cout << "Please enter another method name: ";
	getline(cin, methname);

	try
	  {
	    method m(l.load(methname.c_str()));
	    
	    char s[80];
	    cout << m.fullname(s) << endl;
	    
	    print_row_block((row_block)m);
	    
	    cout << "This method is "
		 << ((m.isregular()) ? "" : "ir")
		 << "regular"
		 << ((m.isdouble()) ? " and double" : "")
		 << ".\n";
	    cout << "Lead head code: " << m.lhcode() << endl;
	  }
	catch (exception &e)
	  {
	    cerr << "Error cannot load method: " << e.what() << endl;
	  }
      }
  }
    
  return 0;
}
 
