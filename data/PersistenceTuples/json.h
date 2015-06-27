/// json.h
/// Shaun Harker
/// 2015-06-27
#ifndef SRH_JSON_H
#define SRH_JSON_H

#include "boost/any.hpp"
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <unordered_set>
#include <cstdint>
#include <cctype>
#include <exception>
#include <stdexcept>

namespace JSON {

// Interface

class Value;
typedef int64_t Integer;
typedef double Double;
typedef std::string String;
typedef std::vector<Value> Array;
typedef std::unordered_map<String,Value> Object;

/// parse
///   Create a JSON object from a string
Value
parse ( std::string const& str );

/// stringify
///   Serialize a JSON object into a string
std::string 
stringify ( Value const& json );

/// load
///   Parse contents of file into JSON object
Value
load ( std::string const& filename );

/// save
///    Serialize contents of JSON object to file
void
save ( Value const& json, std::string const& filename );

// Definitions
class Value : public boost::any {
public:
  using boost::any::any;
  /// Array methods
  Value const& operator [] ( uint64_t i ) const {
    if ( type () == typeid ( Array ) ) {
      Array const& array = boost::any_cast<Array const&> ( *this );
      return array [ i ];
    }
    throw std::runtime_error ( "JSON: Cannot call numeric accessor on non-Array.");
  }
  Value & operator [] ( uint64_t i ) {
    if ( type () == typeid ( Array ) ) {
      Array & array = boost::any_cast<Array &> ( *this );
      return array [ i ];
    }
    throw std::runtime_error ( "JSON: Cannot call numeric accessor on non-Array.");
  }

  /// Object methods
  Value const& operator [] ( std::string const& name ) const {
    if ( type () == typeid ( Object ) ) {
      Object const& object = boost::any_cast<Object const&> ( *this );
      return object . find ( name ) -> second;
    }
    throw std::runtime_error ( "JSON: Cannot call string accessor on non-Object.");
  }
  Value & operator [] ( std::string const& name ) {
    if ( type () == typeid ( Object ) ) {
      Object & object = boost::any_cast<Object &> ( *this );
      return object[name];
    }
    throw std::runtime_error ( "JSON: Cannot call string accessor on non-Object.");
  }

  /// Conversions
  operator String () const { 
    if ( type () == typeid ( String ) ) return boost::any_cast<String>(*this);
    throw std::runtime_error ( "JSON: Cannot cast to string.");
  }
  operator Array () const { 
    if ( type () == typeid ( Array ) ) return boost::any_cast<Array>(*this);
    throw std::runtime_error ( "JSON: Cannot cast to array.");
  }
  operator Object () const { 
    if ( type () == typeid ( Object ) ) return boost::any_cast<Object>(*this);
    throw std::runtime_error ( "JSON: Cannot cast to object.");
  }
  explicit operator Integer () const {
    if ( type () == typeid ( Integer ) ) return boost::any_cast<Integer>(*this);
    throw std::runtime_error ( "JSON: Cannot cast to integer.");
  }
  explicit operator Double () const { 
    if ( type () == typeid ( Double ) ) return boost::any_cast<Double>(*this);
    throw std::runtime_error ( "JSON: Cannot cast to integer.");
  }
};


inline Value
parse ( std::string const& str ) {
  // Build balanced parentheses structure
  std::vector<uint64_t> left;
  std::unordered_map<uint64_t, uint64_t> match;
  std::stack<uint64_t> work_stack;
  bool quote_mode = false;
  bool numeric_mode = false;
  uint64_t N = str . size ();
  for ( uint64_t i = 0; i < N; ++ i ) {
    //std::cout << i << " " << str[i] << "\n";
    if ( numeric_mode ) {
      if ( str [ i ] == ' ' || str [ i ] == ']' || str [ i ] == '}' ) {
        numeric_mode = false;
        //std::cout << "NUMERIC MODE OFF\n";
        match [ work_stack . top () ] = i;
        work_stack . pop ();     
        -- i; // redo this character   
      }
    } else if ( quote_mode ) {
      if ( str [ i ] == '\"' ) { // quote ended
        quote_mode = false;
        //std::cout << "QUOTE MODE OFF\n";
        match [ work_stack . top () ] = i;
        work_stack . pop ();
      }
      if ( str [ i ] == '\\' ) ++ i; // skip escaped character
    } else {
      if ( str[i]=='+' || str[i]=='-' || str[i]=='.' || (str[i] >= '0' && str[i] <= '9') ) {
        work_stack . push ( i );
        left . push_back ( i );
        numeric_mode = true;
        //std::cout << "NUMERIC MODE ON\n";
      }
      if ( str[i] == '\"' ) { // quote begins
        work_stack . push ( i );
        left . push_back ( i );
        quote_mode = true;
        //std::cout << "QUOTE MODE ON\n";
      }
      if ( str[i] == '{' || str[i] == '[' ) { 
        work_stack . push ( i );
        left . push_back ( i );
        //std::cout << "LEFT\n";
      }
      if ( str[i] == '}' || str[i] == ']' ) { 
        //std::cout << "RIGHT MATCH TO " << work_stack . top () << "\n";
        match [ work_stack . top () ] = i;
        if ( work_stack . empty () ||
             (str[work_stack.top()] == '{' && str[i] != '}') ||
             (str[work_stack.top()] == '[' && str[i] != ']')  ) {
          throw std::runtime_error ("JSON::parse: Invalid string.");
        }
        work_stack . pop ();
      }
    }
  }
  if ( not work_stack . empty () ) {
    throw std::runtime_error ("JSON::parse: Invalid string.");
  }

  uint64_t item = 0;
  
  uint64_t num_items = left . size ();
  std::vector<uint64_t> right ( num_items );
  for ( uint64_t i = 0; i < num_items; ++ i ) {
    right [ i ] = match[left[i]];
  }
  left . push_back ( str . size () ); // causes termination

  auto isChild = [&] (uint64_t child, uint64_t parent ) {
    return ( left[child] < right[parent] );
  };

  std::function<Value(void)> 
    parse_object, parse_array, parse_string, parse_numeric, parse_item;
  parse_object = [&](){
    //std::cout << "parse_object begin " << item << "\n";
    uint64_t this_item = item ++;
    Object data;
    bool onkey = true;
    std::string key;
    while ( isChild( item, this_item) ) {
      if ( onkey ) {
        key = str . substr (left[item]+1, right[item]-left[item]-1);
        onkey = false;
        //std::cout << "parse_object key (" << item << ") " << key << "\n";
        ++ item;
      } else {
        data [ key ] = parse_item ();
        onkey = true;
      }
    }
    if ( not onkey ) {
      throw std::runtime_error ("JSON::parse: Invalid string." );
    }
    //std::cout << "parse_object return " << this_item << "\n";
    return data;
  };

  parse_array = [&](){
    //std::cout << "parse_array begin " << item << "\n";
    uint64_t this_item = item ++;
    Array data;
    while ( isChild ( item, this_item ) ) {
      data . push_back ( parse_item () );
    }
    //std::cout << "parse_array return " << this_item << "\n";
    return data;  
  };

  parse_string = [&](){
    std::string data = str . substr (left[item]+1, right[item]-left[item]-1);
    //std::cout << "parse_string (" << item << "):\"" << data << "\"\n";
    ++ item;
    return data;
  };
  parse_numeric = [&](){
    std::string data = str . substr (left[item], right[item]-left[item]);
    if ( std::all_of(data.begin(), data.end(), ::isdigit) ) {
      if ( std::to_string(std::stoll(data)) == data ) {
        ++ item;
        return (Value) (std::stoll(data));
      } 
    }
    ++ item;
    return (Value) (std::stod(data));
  }; 

  parse_item = [&](){
    Value result;
    switch ( str[left[item]] ) {
      case '{' :
        result = parse_object ();
        break;
      case '[' :
        result = parse_array ();
        break;
      case '\"' :
        result = parse_string ();
        break;
      default :
        result = parse_numeric ();
        break;
    }
    return result;
  };

  return parse_item ();
}

inline std::string 
stringify ( Integer i ) {
  return std::to_string ( i );
}

inline std::string 
stringify ( Double d ) {
  return std::to_string ( d );
}

inline std::string 
stringify ( String const& s ) {
  return "\"" + s + "\"";;
}

inline std::string 
stringify ( Array const& a ) {
  std::stringstream ss;
    ss << "["; 
  bool first = true;
  for ( auto val : a ) { 
    if ( first ) first = false; else ss << ",";
    ss << stringify ( val ); 
  }
  ss << "]";
  return ss . str ();
}

inline std::string 
stringify ( Object const& obj ) {
  std::stringstream ss;
  ss << "{"; 
  bool first = true;
  for ( auto const& pair : obj ) { 
    if ( first ) first = false; else ss << ",";
    //ss << "$" << pair.first << "#"; 
    ss << "\"" << pair.first << "\""; 
    ss << ":";
    ss << stringify ( pair.second );
    //ss << "$" << pair.second -> stringify () << "#";
  }
  ss << "}";
  return ss . str (); 
}

inline std::string 
stringify ( Value const& val ) {
  if ( val . type () == typeid ( Object ) ) {
    return stringify ( boost::any_cast<Object const&>(val) );
  }
  if ( val . type () == typeid ( Array ) ) {
    return stringify ( boost::any_cast<Array const&>(val) );
  }
  if ( val . type () == typeid ( String ) ) {
    return stringify ( boost::any_cast<String const&>(val) );
  }
  if ( val . type () == typeid ( Integer ) ) {
    return stringify ( boost::any_cast<Integer>(val) );
  }
  if ( val . type () == typeid ( Double ) ) {
    return stringify ( boost::any_cast<Double>(val) );
  }
  throw std::runtime_error ("JSON::stringify: Invalid value." );
}

inline Value
load ( std::string const& filename ) {
  std::ifstream infile ( filename );
  if ( not infile ) {
    throw std::runtime_error ( "JSON::load. Could not open file " 
                               + filename + " for reading." );
  }
  infile.seekg(0, std::ios::end);
  size_t size = infile.tellg();
  std::string buffer(size, ' ');
  infile.seekg(0);
  infile.read(&buffer[0], size); 
  infile . close ();
  return parse ( buffer );
}

inline void
save ( Value const& json, std::string const& filename ) {
  std::ofstream outfile ( filename );
  if ( not outfile ) {
    throw std::runtime_error ( "JSON::save. Could not open " 
                               + filename + " for writing.");
  }
  outfile << stringify ( json ) << "\n";
  outfile . close ();
}


}

#endif
