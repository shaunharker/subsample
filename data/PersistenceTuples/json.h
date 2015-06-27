/// json.h
/// Shaun Harker
/// 2015-06-27
#ifndef SRH_JSON_H
#define SRH_JSON_H

#include "boost/any.hpp"
#include <vector>
#include <string>
#include <sstream>
#include <unordered_set>
#include <cstdint>

namespace JSON {

// Interface

class Value;
typedef int64_t Integer;
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
    throw std::runtime_error ( "JSON: Cannot call accessor on non-Object.");
  }
  Value & operator [] ( std::string const& name ) {
    if ( type () == typeid ( Object ) ) {
      Object & object = boost::any_cast<Object &> ( *this );
      return object[name];
    }
    throw std::runtime_error ( "JSON: Cannot call accessor on non-Object.");
  }

  /// Integer methods
  operator int32_t () const {
    if ( type () == typeid ( Integer ) ) return boost::any_cast<Integer>(*this);
    throw std::runtime_error ( "JSON: Cannot cast non-integer type to integer.");
  }
  operator int64_t () const { 
    if ( type () == typeid ( Integer ) ) return boost::any_cast<Integer>(*this);
    throw std::runtime_error ( "JSON: Cannot cast non-integer type to integer.");
  }
  operator uint32_t () const { 
    if ( type () == typeid ( Integer ) ) return boost::any_cast<Integer>(*this);
    throw std::runtime_error ( "JSON: Cannot cast non-integer type to integer.");
  }
  operator uint64_t () const { 
    if ( type () == typeid ( Integer ) ) return boost::any_cast<Integer>(*this);
    throw std::runtime_error ( "JSON: Cannot cast non-integer type to integer.");
  }

  /// String methods
  operator std::string () const { 
    if ( type () == typeid ( String ) ) return boost::any_cast<String>(*this);
    throw std::runtime_error ( "JSON: Cannot cast non-string type to string.");
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
      if ( str [ i ] < '0' || str [ i ] > '9' ) {
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
      if ( str[i] >= '0' && str[i] <= '9' ) {
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
    parse_object, parse_array, parse_string, parse_integer, parse_item;
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
  parse_integer = [&](){
    std::string data = str . substr (left[item], right[item]-left[item]);
    //std::cout << "parse_integer (" << item << "):\"" << data << "\"\n";
    ++ item;
    return Integer (std::stoll(data));
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
        result = parse_integer ();
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
    return stringify ( boost::any_cast<Object const&> val );
  }
  if ( val . type () == typeid ( Array ) ) {
    return stringify ( boost::any_cast<Array const&> val );
  }
  if ( val . type () == typeid ( String ) ) {
    return stringify ( boost::any_cast<String const&> val );
  }
  if ( val . type () == typeid ( Integer ) ) {
    return stringify ( boost::any_cast<Integer> val );
  }
  throw std::runtime_error ("JSON::stringify: Invalid value." );
}

}

#endif
