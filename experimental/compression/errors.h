// (C) Copyright 2008 CodeRage, LLC (turkanis at coderage dot com)
// (C) Copyright 2003-2007 Jonathan Turkanis
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt.)

// See http://www.boost.org/libs/iostreams for documentation.

// Note: custom allocators are not supported on VC6, since that compiler
// had trouble finding the function zlib_base::do_init.

//FIXME:typedef void* (*xalloc_func)(void*, zlib::uint, zlib::uint);
//FIXME:typedef void (*xfree_func)(void*, void*);

class ZLibError 
{
public:
  explicit ZLibError(int error)
    : m_error(error) {}

public:
  int getCode() const
  { 
    return m_error; 
  }

private:
  int m_error;
}; //class ZLibError;

class GZipError 
{
public:
  explicit GZipError(int error)
    : m_error(error), 
      m_zlibErrorCode(ZLibOKay) { }

  explicit GZipError(const ZLibError& e)
    : m_error(GZipZLibError), 
      m_zlibErrorCode(e.getCode()) {}

public:
  int getCode() const
  {
    return m_error;
  }

  int getZLibErrorCode() const 
  { 
    return m_ZLibErrorCode;
  }

private:
  int m_error;
  int m_ZLibErrorCode_;
};
