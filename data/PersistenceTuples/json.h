/// json.h
/// Shaun Harker
/// 2015-06-27
#ifndef SRH_JSON_H
#define SRH_JSON_H

#include "boost/variant.hpp"
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <unordered_set>
#include <cstdint>
#include <cctype>
#include <exception>
#include <stdexcept>
#include <typeinfo>

using namespace std::string_literals;

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
class Value {
public:
  //enum Type { Integer=0, Double=1, String=2, Array=3, Object=4 };
  // Construction
  Value () {}
  Value ( Integer const& x ) { data_ = x; }
  Value ( Double const& x ) { data_ = x; }
  Value ( String const& x ) { data_ = x; }
  Value ( Array const& x ) { data_ = x; }
  Value ( Object const& x ) { data_ = x; }  
  Value & operator = ( Integer const& x ) { data_ = x; return *this; }
  Value & operator = ( Double const& x ) { data_ = x; return *this; }
  Value & operator = ( String const& x ) { data_ = x; return *this; }
  Value & operator = ( Array const& x ) { data_ = x; return *this; }
  Value & operator = ( Object const& x ) { data_ = x; return *this; }
  // Conversions. Note: due to problems I had to make "Double" explicit
  operator Integer & () { return boost::get<Integer>(data_); }
  explicit operator Double & () { return boost::get<Double>(data_); }
  operator String & () { return boost::get<String>(data_); }
  operator Array & () { return boost::get<Array>(data_); }
  operator Object & () { return boost::get<Object>(data_); }
  operator Integer const& () const { return boost::get<Integer>(data_); }
  explicit operator Double const& () const { return boost::get<Double>(data_); }
  operator String const& () const { return boost::get<String>(data_); }
  operator Array const& () const { return boost::get<Array>(data_); }
  operator Object const& () const { return boost::get<Object>(data_); }
  // Accessors
  Value & operator [] ( std::string const& key ) { return ((Object&)(*this))[key]; }
  Value const& operator [] ( std::string const& key ) const { return ((Object const&)(*this)).find(key)->second; }
  Value & operator [] ( int64_t i ) { return ((Array&)(*this))[i]; }
  Value const& operator [] ( int64_t i ) const { return ((Array const&)(*this))[i]; }
  // Creation from C++ types
  template < class T >
  Value & operator = ( std::vector<T> const& x ) {
    *this = Array ();
    uint64_t N = x . size ();
    ((Array&)(*this)) . resize ( N );
    for ( uint64_t i = 0; i < N; ++ i ) {
      ((Array&)(*this))[i] = x[i];
    }
    return *this;
  }
  template < class T >
  Value & operator = ( std::unordered_map<std::string,T> const& x ) {
    *this = Object ();
    for ( auto const& pair : x ) {
      ((Object&)(*this))[pair.first] = pair.second;
    }
    return *this;
  }  
  // delegate visitors
  template <typename Visitor>
  BOOST_VARIANT_AUX_GENERIC_RESULT_TYPE(typename Visitor::result_type)
  apply_visitor(Visitor& visitor) { 
    return data_ . apply_visitor ( visitor );
  }
  template <typename Visitor>
  BOOST_VARIANT_AUX_GENERIC_RESULT_TYPE(typename Visitor::result_type)
  apply_visitor(Visitor& visitor) const { 
    return data_ . apply_visitor ( visitor );
  }
private:
  boost::variant<Integer, Double, String, Array, Object> data_;
};

inline Value
parse ( std::string const& str ) {
  // Build balanced parentheses structure
  auto is_numeric = [](char c){ 
    if ( std::isspace(c) ) return false;
    return c != '\"' && c != '[' && c != ']' 
        && c != '{' & c != '}' && c != ',' && c != ':';
//  return c =='+' || c =='-' || c =='.' || (c >= '0' && c <= '9');
  };

  std::vector<uint64_t> left;
  std::unordered_map<uint64_t, uint64_t> match;
  std::stack<uint64_t> work_stack;
  bool quote_mode = false;
  bool numeric_mode = false;
  uint64_t N = str . size ();
  for ( uint64_t i = 0; i < N; ++ i ) {
    //std::cout << i << " " << str[i] << "\n";
    if ( numeric_mode ) {
      if ( not is_numeric ( str[i] ) ) {
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
      if ( is_numeric ( str[i] ) ) {
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
          throw std::runtime_error ("JSON::parse: Invalid string (1).");
        }
        work_stack . pop ();
      }
    }
  }
  if ( not work_stack . empty () ) {
    throw std::runtime_error ("JSON::parse: Invalid string (2).");
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
      throw std::runtime_error ("JSON::parse: Invalid string (3)." );
    }
    //std::cout << "parse_object return " << this_item << "\n";
    return Value(data);
  };

  parse_array = [&](){
    //std::cout << "parse_array begin " << item << "\n";
    uint64_t this_item = item ++;
    Array data;
    while ( isChild ( item, this_item ) ) {
      data . push_back ( parse_item () );
    }
    //std::cout << "parse_array return " << this_item << "\n";
    //std::cout << "parse_array return (cont): " << stringify(data) << "\n";
    return Value(data);  
  };

  parse_string = [&](){
    std::string data = str . substr (left[item]+1, right[item]-left[item]-1);
    //std::cout << "parse_string (" << item << "):\"" << data << "\"\n";
    ++ item;
    return Value(data);
  };
  parse_numeric = [&](){
    std::string data = str . substr (left[item], right[item]-left[item]);
    if ( std::all_of(data.begin(), data.end(), ::isdigit) ) {
      if ( std::to_string(std::stoll(data)) == data ) {
        ++ item;
        return Value(std::stoll(data));
      } 
    }
    ++ item;
    //std::cout << "Double: \"" << data << "\"\n";
    return Value(std::stod(data));
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
    //std::cout << "parse_item: returning with " << stringify(result) << "\n";
    return result;
  };

  return parse_item ();
}

class stringify_visitor : public boost::static_visitor<std::string> {
public:
  /// stringify Integer
  std::string operator()(Integer const& i) const {
    //std::cout << "stringify(Integer)\n";
    return std::to_string ( i );
  }
  /// stringify Double
  std::string operator()(Double const& d) const {
    //std::cout << "stringify(Double)\n";
    return std::to_string ( d );
  }
  /// stringify String
  std::string operator()(String const& s) const {
    //std::cout << "stringify(String)\n";
    return "\"" + s + "\"";
  }
  /// stringify Array
  std::string operator()(Array const& a) const {
    //std::cout << "stringify(Array)\n";
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
  /// stringify Object
  std::string operator()(Object const& obj) const {
    //std::cout << "stringify(Object)\n";
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
};

inline std::string 
stringify ( Value const& val ) {
  //std::cout << "stringify(Value)\n";
  return boost::apply_visitor ( stringify_visitor (), val );
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
  //std::cout << "Buffer=\"" << buffer << "\"\n";
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
