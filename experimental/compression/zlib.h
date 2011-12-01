// (C) Copyright 2008 CodeRage, LLC (turkanis at coderage dot com)
// (C) Copyright 2003-2007 Jonathan Turkanis
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt.)

// See http://www.boost.org/libs/iostreams for documentation.

// Note: custom allocators are not supported on VC6, since that compiler
// had trouble finding the function zlib_base::do_init.

//FIXME:typedef void* (*xalloc_func)(void*, zlib::uint, zlib::uint);
//FIXME:typedef void (*xfree_func)(void*, void*);



//FIXME:typedef void* (*xalloc_func)(void*, zlib::uint, zlib::uint);
//FIXME:typedef void (*xfree_func)(void*, void*);

template<typename Alloc>
struct zlib_allocator_traits 
{
#ifndef BOOST_NO_STD_ALLOCATOR
  typedef typename Alloc::template rebind<char>::other type;
#else
  typedef std::allocator<char> type;
#endif
};

template< typename Alloc,
          typename Base = // VC6 workaround (C2516)
	  BOOST_DEDUCED_TYPENAME zlib_allocator_traits<Alloc>::type >
struct zlib_allocator : private Base 
{
private:
  typedef typename Base::size_type size_type;

public:
  BOOST_STATIC_CONSTANT(bool, custom = 
			(!is_same<std::allocator<char>, Base>::value));
  typedef typename zlib_allocator_traits<Alloc>::type allocator_type;
  static void* allocate(void* self, zlib::uint items, zlib::uint size);
  static void deallocate(void* self, void* address);
};

//zlib_base;

struct BasicZlibCompressor 
  : symmetric_filter<detail::zlib_compressor_impl<Alloc>, Alloc> 
{
private:
  typedef detail::zlib_compressor_impl<Alloc>         impl_type;
  typedef symmetric_filter<impl_type, Alloc>  base_type;

public:
  typedef typename base_type::char_type               char_type;
  typedef typename base_type::category                category;
  basic_zlib_compressor( const zlib_params& = zlib::default_compression, 
			 int buffer_size = default_device_buffer_size );
  zlib::ulong crc() { return this->filter().crc(); }
  int total_in() {  return this->filter().total_in(); }
};
BOOST_IOSTREAMS_PIPABLE(basic_zlib_compressor, 1)

typedef basic_zlib_compressor<> zlib_compressor;

template<typename Alloc = std::allocator<char> >
struct basic_zlib_decompressor 
    : symmetric_filter<detail::zlib_decompressor_impl<Alloc>, Alloc> 
{
private:
  typedef detail::zlib_decompressor_impl<Alloc>       impl_type;
  typedef symmetric_filter<impl_type, Alloc>  base_type;

public:
  typedef typename base_type::char_type               char_type;
  typedef typename base_type::category                category;
  basic_zlib_decompressor( int window_bits = zlib::default_window_bits,
			   int buffer_size = default_device_buffer_size );
  basic_zlib_decompressor( const zlib_params& p,
			   int buffer_size = default_device_buffer_size );
  zlib_ulong crc() { return this->filter().crc(); }
  int total_out() {  return this->filter().total_out(); }
  bool eof() { return this->filter().eof(); }
};
BOOST_IOSTREAMS_PIPABLE(basic_zlib_decompressor, 1)

typedef basic_zlib_decompressor<> zlib_decompressor;


template<typename Alloc>
basic_zlib_compressor<Alloc>::basic_zlib_compressor
(const zlib_params& p, int buffer_size) 
  : base_type(buffer_size, p) 
{ 
}

template<typename Alloc>
basic_zlib_decompressor<Alloc>::basic_zlib_decompressor
(int window_bits, int buffer_size) 
  : base_type(buffer_size, window_bits) { }

template<typename Alloc>
basic_zlib_decompressor<Alloc>::basic_zlib_decompressor
(const zlib_params& p, int buffer_size) 
  : base_type(buffer_size, p) { }



