// -*- C++ -*- splices.cpp - utility to find splices between methods
// Copyright (C) 2010 Richard Smith <richard@ex-parrot.com>

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

#include <ringing/common.h>

#include <ringing/mathutils.h>
#include <ringing/streamutils.h>
#include <ringing/methodset.h>
#include <ringing/litelib.h>
#include <ringing/group.h>
#include <ringing/falseness.h>
#include "args.h"

#include <iostream>
#include <list>
#include <set>
#include <utility>

RINGING_USING_NAMESPACE
RINGING_USING_STD

struct arguments
{
  init_val<int,0>      bells;

  init_val<int,-1>     only_n_leads;
  init_val<bool,false> null_splices;
  init_val<bool,false> same_le;

  init_val<bool,false> show_pn;
  init_val<bool,false> group_splices;

  init_val<bool,false> in_course;

  arguments( int argc, char *argv[] );
  void bind( arg_parser& p );
  bool validate( arg_parser& p );
};

arguments::arguments( int argc, char *argv[] )
{
  arg_parser ap( argv[0], "musgrep -- grep rows for music", "OPTIONS" );
  bind( ap );

  if ( !ap.parse(argc, argv) )
    {
      ap.usage();
      exit(1);
    }
  
  if ( !validate(ap) )
    exit(1);
}

void arguments::bind( arg_parser& p )
{
  p.add( new help_opt );
  p.add( new version_opt );
           
  p.add( new integer_opt
         ( 'b', "bells",
           "The number of bells.  This option is required", "BELLS",
           bells ) );

  p.add( new boolean_opt
         ( 'n', "null-splice",
           "Display pairs of methods with no splice", 
           null_splices ) );

  p.add( new boolean_opt
         ( 'i', "in-course",
           "Only display splices for in-course lead heads", 
           in_course ) );

  p.add( new boolean_opt
         ( 'e', "same-lead-ends",
           "Only look at pairs of methods with the same lead end change",
           same_le ) );

  p.add( new boolean_opt
         ( 'p', "place-notation",
           "Use place notations instead of names", 
           show_pn ) );

  p.add( new boolean_opt
         ( 'g', "group",
           "Group together methods with mutual splices", 
           group_splices ) );

  p.add( new integer_opt
         ( 'l', "leads",
           "Only display methods with a NUM-lead splice", "NUM",
           only_n_leads ) );
}

bool arguments::validate( arg_parser& ap )
{               
  if ( bells <= 0 )
    {
      ap.error( "Then number of bells must be positive" );
      return false;
    }

  if ( bells >= int(bell::MAX_BELLS) )
    {
      ap.error( make_string() << "The number of bells must be less than "
                << bell::MAX_BELLS );
      return false;
    }

  if ( null_splices && only_n_leads != -1 ) 
    {
      ap.error( "The -l and -n options are incompatible" );
      return false;
    }

  if ( null_splices && group_splices ) 
    {
      ap.error( "The -g and -n options are incompatible" );
      return false;
    }

  return true;
}

class splices {
public:
  splices( arguments const& args ) : args(args) {}

  void find_splices( library const& lib );

private:
  void test_splice( method const& a, method const& b );
  string describe_splice( group const& sg ) const;
  void save_splice( string const& a, string const& b, string const& d );
  void print_splice_groups() const;

  typedef list< pair< string, set<string> > > table_type;
  table_type table;
  arguments const& args;
};

void splices::print_splice_groups() const
{
  for ( table_type::const_iterator i=table.begin(), e=table.end(); i!=e; ++i ) {
    bool need_sep = false;
    for ( set<string>::const_iterator 
            i2=i->second.begin(), e2=i->second.end(); i2 != e2; ++i2 ) {
      if (need_sep) cout << " / ";
      cout << *i2; 
      need_sep = true;
    }
    cout << "\t" << i->first << "\n";
  }
}

void splices::save_splice( string const& a, string const& b, string const& d )
{
  if ( !args.group_splices ) {
    cout << a << " / " << b << "\t" << d << "\n";
    return;
  }
  
  table_type::iterator found = table.end();
  for ( table_type::iterator i=table.begin(), e=table.end(); i!=e;  )
    if ( d == i->first && 
         ( i->second.find(a) != i->second.end() ||
           i->second.find(b) != i->second.end() ) ) 
    {
      if ( found == e ) {
        i->second.insert(a);
        i->second.insert(b); 
        found = i++;
      }
      else {
        found->second.insert( i->second.begin(), i->second.end() );
        found->second.insert(a);
        found->second.insert(b);
        table.erase(i++);
      }
    }
    else ++i;

  if ( found == table.end() ) {
    set<string> meths;
    meths.insert(a);  meths.insert(b);
    table.push_back( make_pair( d, meths ) );
  }
}
 
string splices::describe_splice( group const& sg ) const
{
  return make_string() << (sg.size()/2) << "-lead";
}

void splices::test_splice( method const& a, method const& b )
{
  if ( args.same_le && a.back() != b.back() )
    return;
 
  int flags = 0; 
  if ( args.in_course ) 
    flags |= falseness_table::in_course_only;
  group sg( falseness_table( a, b, flags ).generate_group() );

  if ( !args.null_splices &&
       sg.size() == factorial(args.bells-1) / (args.in_course ? 2 : 1) )
    return;

  if ( args.only_n_leads != -1 && sg.size() != args.only_n_leads * 2 )
    return;

  // If we have no names, use place notations
  string astr = a.name(), bstr = b.name();
  if ( astr.empty() || args.show_pn ) 
    astr = a.format( method::M_FULL_SYMMETRY | method::M_DASH );
  if ( bstr.empty() || args.show_pn ) 
    bstr = b.format( method::M_FULL_SYMMETRY | method::M_DASH );

  save_splice( astr, bstr, describe_splice(sg) );
}

void splices::find_splices( library const& lib )
{
  for ( library::const_iterator i=lib.begin(), e=lib.end(); i!=e; ++i ) 
  {
    library::const_iterator j=i;  ++j;
    for ( ; j != e; ++j ) 
    {
      method a( i->meth() ), b( j->meth() );  

      // Use litelib payloads as a proxy for the name as there's no way
      // for a litelib to unambiguously carry a name.
      if ( i->has_facet< litelib::payload >() )
        a.name( i->get_facet< litelib::payload >() );
      if ( j->has_facet< litelib::payload >() )
        b.name( j->get_facet< litelib::payload >() );

      test_splice( a, b );
    }
  }

  if ( args.group_splices ) 
    print_splice_groups();
}

int main( int argc, char *argv[] )
{
  arguments args(argc, argv);

  // Read methods into memory -- we need to make multiple passes over
  // the list and we can't do that on standard input.
  methodset meths;
  {
    meths.store_facet< litelib::payload >();
    litelib in( args.bells, cin );
    copy( in.begin(), in.end(), back_inserter(meths) );  
  }

  splices spl( args );
  spl.find_splices( meths );
}
