#include "byte_stream.hh"
#include "debug.hh"

using namespace std;

ByteStream::ByteStream( uint64_t capacity ) : capacity_( capacity ) {
  
}

// Push data to stream, but only as much as available capacity allows.
void Writer::push( string data )
{
  if(error_ || is_closed_) {
    return;
  }
  uint64_t can_write = available_capacity();
  uint64_t to_write = min( can_write, static_cast<uint64_t>( data.size() ) );
  buffer_.append( data.data(), to_write );
  total_bytes_pushed_ += to_write;
}

// Signal that the stream has reached its ending. Nothing more will be written.
void Writer::close()
{
  is_closed_ = true;
}

// Has the stream been closed?
bool Writer::is_closed() const
{
  return is_closed_;
}

// How many bytes can be pushed to the stream right now?
uint64_t Writer::available_capacity() const
{
  if( buffer_.size() >= capacity_ ) {
    return 0;
  }
  return capacity_ - buffer_.size();
}

// Total number of bytes cumulatively pushed to the stream
uint64_t Writer::bytes_pushed() const
{
  return total_bytes_pushed_;
}

// Peek at the next bytes in the buffer -- ideally as many as possible.
// It's not required to return a string_view of the *whole* buffer, but
// if the peeked string_view is only one byte at a time, it will probably force
// the caller to do a lot of extra work.
string_view Reader::peek() const
{
  return string_view(buffer_);
}

// Remove `len` bytes from the buffer.
void Reader::pop( uint64_t len )
{
  uint64_t to_pop = min( len, static_cast<uint64_t>(buffer_.size()) );
  buffer_.erase(0, to_pop);
  total_bytes_popped_ += to_pop;
}

// Is the stream finished (closed and fully popped)?
bool Reader::is_finished() const
{
  return is_closed_ && buffer_.empty();
}

// Number of bytes currently buffered (pushed and not popped)
uint64_t Reader::bytes_buffered() const
{
  return buffer_.size();
}

// Total number of bytes cumulatively popped from stream
uint64_t Reader::bytes_popped() const
{
  return total_bytes_popped_;
}
