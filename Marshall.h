/*
 * =====================================================================================
 *
 *       Filename:  Marshall.h
 *
 *    Description:  to serialize data into buff
 *
 *        Version:  1.0
 *        Created:  2012年09月18日 18时56分03秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yuan DaXing (http://goo.gl/VpwzO), mryuan0@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef  MARSHALL_H__INC
#define  MARSHALL_H__INC

#include <iostream>
#include <stdlib.h>
#include <vector>
#include <list>
#include <map>
#include <assert.h>
#include <string.h>
#include <string>
#include <tr1/unordered_map>
#include <map>
#include <set>
#include <stdint.h>
/* 
 * objective: 
 * 				portable, support multi data type, easy to cooperate with send, read,
 *            compression, error detecting
 * implement: 
 * 			1. for portable, use little endian encoding, to support different cpu-byte transfer, use 
 * stdint u8, u32, u64, we do not support int, long, short, you can select some type to convert safely.
 * 			2. for multi data type, we support vector, list, set, map, unordered_map
 * 			3. to easy cooperate with send, read, the data structure in the Marshall is
 * 			   ||u32||data||, and supply complete, getBuff, getBuffSize interface
 * 		    4. currently we do not consider compression much, just use variant integer encoding
 * 		    5. error detecting, append CRC32 at the end of the buff, not implement because TCP
 * 		       is already safe enough.
 * 		    6. actually, this implementation is very efficiency.
 *
 * the Marshall class manage it's memory, if you use the reference to marshall's buf
 * you should ensure the Marshall exist, do not use the pack function directly if you 
 * are not sure, the operator<< function is enough for you.
 *
 * UnMarshall class do not take the memory when construct, you should free it you self.
 * the data format is 
 * |TotalSize|data|
 * TotalSize = 4 + dataSize
 */
enum { headerSize = 4, defaultSize = 1024 };
void packHeader(char *buff, int size) {
	unsigned u = size;
	for( int i = 0 ; i < headerSize ; i++ )
	{
		unsigned char uc = u & 0xff;
		buff[i] = static_cast<char>(uc);
		u = u >> 8;
	}
}

bool checkComplete(char *buff, int dataSize) {
	if(dataSize < headerSize)
		return false;
	int factor = 1;
	int sum = 0;
	for( int i = 0 ; i < headerSize ; i++ )
	{
		//the char is unsigned char
		sum += static_cast<unsigned char>(buff[i]) 
			* factor;
		factor <<= 8;
	}
	return sum == dataSize ;
}
class Marshall {
	private:
		char *buf_;
		int cur_, capacity_;

		void ensureSpace(int byte) {
			if( byte + cur_ > capacity_ )
			{
				capacity_ = std::max(cur_ + 2 * byte, 2 * capacity_);

				char *newbuf = new char[capacity_];
				assert( newbuf != NULL );
				memmove(newbuf, buf_, cur_);
				delete cur_;
				cur_ = newbuf;
			}
		}

		//no allowed copy
		Marshall(const Marshall&);
		Marshall & operator=(const Marshall&);
	public:
		//reserve 4 bytes to store the buff size
		Marshall() : buf_(NULL), cur_(0), capacity_(0) {
			buf_ = new char[defaultSize] ;
			assert(buf_ != NULL);
			cur_ = headerSize;
			capacity_ = defaultSize;
		}
		Marshall(char *buff, int dataSize, int capacity) :
			buf_(buff), cur_(dataSize), capacity_(capacity) {
				assert(checkComplete(buf_, cur_));
			}

		~Marshall( ) {
			if(buf_)
				delete buf_;
		}

		int contentByte() {
			return cur_ - headerSize;
		}

		void takeBuff(char * &buff, int &dataSize, int &capacity) {
			complete();
			buff = buf_;
			dataSize = cur_;
			capacity = capacity_;
		}
		//suggest you do not use member function below directly
		void packByte(char c) {
			packRawMem(&c, sizeof(char));
		}	

		void packRawMem(const char *b, int size) {
			ensureSpace(size);
			memmove(buf_ + cur_, b, size);
			cur_ += size;
		}
		void complete() {
			packHeader(buf_, cur_);
		}
};

 
Marshall & operator<<(Marshall &, bool);
Marshall & operator<<(Marshall &, uint8_t u8);
Marshall & operator<<(Marshall &, int8_t s8);
Marshall & operator<<(Marshall &, uint16_t u16);
Marshall & operator<<(Marshall &, int16_t s16);
Marshall & operator<<(Marshall &, uint32_t u32 );
Marshall & operator<<(Marshall &, int32_t s32 );
Marshall & operator<<(Marshall &, uint64_t u32 );
Marshall & operator<<(Marshall &, int64_t s32 );
Marshall & operator<<(Marshall &, const std::string &);

template <typename IterType>
Marshall & packRange(Marshall &, IterType beg, IterType end);
//please notice, T only support bool uint8/16/32/64_t 
template <typename T>
Marshall & operator<<(Marshall &, const std::vector<T> &);
template <typename T>
Marshall & operator<<(Marshall &, const std::list<T> &);
template <typename T>
Marshall & operator<<(Marshall &, const std::set<T> &);
template <typename T, typename V>
Marshall & operator<<(Marshall &, const std::map<T, V> &);
template <typename T, typename V>
Marshall & operator<<(Marshall &, const std::tr1::unordered_map<T, V> &);

class UnMarshall {
	private:		
		char *buf_;
		int cur_;
		int dataSize_, capacity_;	
		//no copy
		UnMarshall(const UnMarshall &);
		UnMarshall & operator=(const UnMarshall &);
	public:
		UnMarshall(char *buf, int dataSize, int capacity) :
		   buf_(buf), cur_(headerSize), dataSize_(dataSize), 
		   capacity_(capacity) { 
			   assert( checkComplete(buf_, dataSize_) == true );
		   }
		~UnMarshall() {
			delete buf_;
		}


		char takeChar( ) {
			return buf_[cur_++];
		}

		const char * getBufferPtr(int size) {
			const char *ret = buf_ + cur_;
			cur_ += size;
			return ret;
		}

		int contentByte() { return dataSize_ - cur_; }

		void takeLeftBuff(char * &buff, int &dataSize, 
				int &capacity) {
			int left = contentByte();
			memmove(buf_ + headerSize, buf_ + cur_, left);
			buff = buf_;
			dataSize = left + headerSize;
			capacity = capacity_;
			packHeader(buff, dataSize);

			buf_ = NULL;
			dataSize_ = capacity_ = cur_ = 0;
		}
};
//the same usage as the above

UnMarshall & operator>>(UnMarshall &, bool &);
UnMarshall & operator>>(UnMarshall &, uint8_t &);
UnMarshall & operator>>(UnMarshall &, int8_t &);
UnMarshall & operator>>(UnMarshall &, uint16_t &);
UnMarshall & operator>>(UnMarshall &, int16_t &);
UnMarshall & operator>>(UnMarshall &, uint32_t &);
UnMarshall & operator>>(UnMarshall &, int32_t &);
UnMarshall & operator>>(UnMarshall &, uint64_t &);
UnMarshall & operator>>(UnMarshall &, int64_t &);
UnMarshall & operator>>(UnMarshall &, std::string &);
template <typename T>
UnMarshall & operator>>(UnMarshall &, std::vector<T> &);
template <typename T>
UnMarshall & operator>>(UnMarshall &, std::list<T> &);
template <typename T>
UnMarshall & operator>>(UnMarshall &, std::set<T> &);
template <typename T, typename V>
UnMarshall & operator>>(UnMarshall &, std::map<T, V> &);
template <typename T, typename V>
UnMarshall & operator>>(UnMarshall &, std::tr1::unordered_map<T, V> &);





Marshall & operator<<(Marshall &m, bool b) {
	m.packByte(static_cast<char>(b));
	return m;
}
Marshall & operator<<(Marshall & m, uint8_t u8) {
	m.packByte(static_cast<char>(u8));
	return m;
}
Marshall & operator<<(Marshall &m, int8_t s8) {
	return m << static_cast<uint8_t>(s8);
}
Marshall & operator<<(Marshall &m, uint16_t u16) {
	return m << static_cast<uint8_t>(u16 & 0xff)
			<< static_cast<uint8_t>(u16 >> 8);
}
Marshall & operator<<(Marshall &m, int16_t s16) {
	return m << static_cast<uint16_t>(s16);
}
Marshall & operator<<(Marshall & m, uint32_t u32) {
	return m << static_cast<uint16_t>(u32 & 0xffff)
			<< static_cast<uint16_t>(u32 >> 16);
}
Marshall & operator<<(Marshall &m, int32_t s32) {
	return m << static_cast<uint32_t>(s32);
}
Marshall & operator<<(Marshall & m, uint64_t u64) {
	return m << static_cast<uint32_t>(u64 & 0xffffffff)
			<< static_cast<uint32_t>(u64  >> 32);
}
Marshall & operator<<(Marshall &m, int64_t s64) {
	return m << static_cast<uint64_t>(s64);
}
Marshall & operator<<(Marshall &m, const std::string &s) {
	uint32_t size = static_cast<uint32_t>(s.size());
	m << size;
	m.packRawMem(s.data(), size);
	return m;
}
template <typename T, typename V>
Marshall & operator<<(Marshall &m, const std::pair<T,V> &pair) {
	return m << pair.first << pair.second;
}
template <typename IterType>
Marshall & packRange(Marshall &m, IterType beg, IterType end) {
	for(  ; beg != end ; ++beg )
	{
		m << *beg ;
	}
	return m;	
}
template <typename T>
Marshall & operator<<(Marshall &m, const std::vector<T> &v) {
	uint32_t size = static_cast<uint32_t>(v.size());
	m << size ;
	return packRange(m, v.begin(), v.end());
}
template <typename T>
Marshall & operator<<(Marshall &m, const std::list<T> &l){
	uint32_t size = static_cast<uint32_t>(l.size());
	m << size ;
	return packRange(m, l.begin(), l.end());
}
template <typename T>
Marshall & operator<<(Marshall &m, const std::set<T> &s){
	uint32_t size = static_cast<uint32_t>(s.size());
	m << size ;
	return packRange(m, s.begin(), s.end());
}
template <typename T, typename V>
Marshall & operator<<(Marshall &m, const std::map<T, V> &map){
	uint32_t size = static_cast<uint32_t>(map.size());
	m << size ;
	return packRange(m, map.begin(), map.end());
}
template <typename T, typename V>
Marshall & operator<<(Marshall &m, const std::tr1::unordered_map<T, V> &umap) {
	uint32_t size = static_cast<uint32_t>(umap.size());
	m << size ;
	return packRange(m, umap.begin(), umap.end());
}




/* 
 * the same as above
 */
UnMarshall & operator>>(UnMarshall &um, bool &b) {
	char c = um.takeChar();
	b = (c > 0);
	return um;
}

UnMarshall & operator>>(UnMarshall &um, uint8_t &u8) {
	char c;
	c = um.takeChar();
	u8 = static_cast<uint8_t>(c);
	return um;
}

UnMarshall & operator>>(UnMarshall &um, int8_t &s8) {
	uint8_t u8;
	um >> u8;
	s8 = static_cast<int8_t>(u8);
	return um;
}

UnMarshall & operator>>(UnMarshall &um, uint16_t &u16) {
	uint8_t h, l;
	um >> l >> h;
	u16 = static_cast<uint16_t>(l) + 
		(static_cast<uint16_t>(h) << 8) ;
	return um;
}
UnMarshall & operator>>(UnMarshall &um, int16_t &s16) {
	uint16_t u16;
	um >> u16;
	s16 = static_cast<int16_t>(u16);
	return um;
}

UnMarshall & operator>>(UnMarshall &um, uint32_t &u32) {
	uint16_t h, l;
	um >> l >> h;
	u32 = static_cast<uint32_t>(l) + 
		(static_cast<uint32_t>(h) << 16) ;
	return um;
}
UnMarshall & operator>>(UnMarshall &um, int32_t &s32) {
	uint32_t u32;
	um >> u32;
	s32 = static_cast<int32_t>(u32);
	return um;
}
UnMarshall & operator>>(UnMarshall &um, uint64_t &u64) {
	uint32_t h, l;
	um >> l >> h;
	u64 = static_cast<uint64_t>(l) + 
		(static_cast<uint64_t>(h) << 32) ;
	return um;
}
UnMarshall & operator>>(UnMarshall &um, int64_t &s64) {
	uint64_t u64;
	um >> u64;
	s64 = static_cast<int64_t>(u64);
	return um;
}
UnMarshall & operator>>(UnMarshall &um, std::string &s) {
	uint32_t  size;
	um >> size;
	const char *start = um.getBufferPtr(size);
	std::string tmp(start, start+size);
	s = tmp;
	return um;
}
template <typename T>
UnMarshall & operator>>(UnMarshall &um, std::vector<T> &v) {
	uint32_t size;
	um >> size;

	for( uint32_t i = 0 ; i < size ; i++ )
	{
		T t;
		um >> t;
		v.push_back(t);
	}
	return um;
}
template <typename T>
UnMarshall & operator>>(UnMarshall &um, std::list<T> &list) {
	uint32_t size;
	um >> size;
	for( uint32_t i = 0 ; i < size ; i++ )
	{
		T t;
		um >> t;
		list.push_back(t);
	}
	return um;
}
template <typename T>
UnMarshall & operator>>(UnMarshall &um, std::set<T> &set) {
	uint32_t size;
	um >> size;
	for( uint32_t i = 0 ; i < size ; i++ )
	{
		T t;
		um >> t;
		set.insert(t);
	}
	return um;

}
template <typename T, typename V>
UnMarshall & operator>>(UnMarshall &um, std::pair<T, V> &pair) {
	return um >> pair.first >> pair.second;
}
template <typename T, typename V>
UnMarshall & operator>>(UnMarshall &um, std::map<T, V> &map) {
	uint32_t size;
	um >> size;
	for( uint32_t i = 0 ; i < size ; i++ )
	{
		std::pair<T, V> pair;
		um >> pair;
		map.insert(pair);
	}
	return um;
}
template <typename T, typename V>
UnMarshall & operator>>(UnMarshall &um, std::tr1::unordered_map<T, V> &umap) {
	uint32_t size;
	um >> size;
	for( uint32_t i = 0 ; i < size ; i++ )
	{
		std::pair<T, V> pair;
		um >> pair;
		umap.insert(pair);
	}
	return um;
}

#endif   /* ----- #ifndef MARSHALL_H__INC  ----- */
