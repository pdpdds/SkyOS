/*
 *	hash_map.h
 *    
 *	Copyright (c) 2014, Long(Ryan) Nangong.
 *      All right reserved.
 *
 *      Email: lnangong@hawk.iit.edu
 *      Created on: July 15, 2014*
 */

#ifndef _HASH_MAP_H_
#define _HASH_MAP_H_		//Header guard


template <typename Key, typename Value>
class hash_map { 			//Hash_map interface
private:
	class entry{			//Entry node
	public:
        	Key _key;              	//Entry key
        	Value _value;          	//Entry value
        	entry* next;          	//next entry
        	entry* prev;          	//previous entry
	};

public:
	class iterator{			//Hash_map iterator
	public:
		iterator(entry* bucket);
        	iterator& operator++();
        	iterator& operator--();
		bool operator==(const iterator& p) const;
		bool operator!=(const iterator& p) const;
		Value& operator*();
		friend class hash_map;
	
	private:
        	entry* _bucket;
	};
	
public:					//Hash map member functions
	hash_map();
	~hash_map();
	size_t hash (const Key& key);
	size_t size();
	iterator begin();
	iterator end();
	iterator rbegin();
	iterator rend();
	iterator insert (const Key& key, const Value& value); 
	iterator find (const Key& key);
	void erase (iterator pos); 
	Value operator[] (const Key& key);

private:					//Hash map private variable
	static const int capacity=100;
	entry* HashTable[capacity];
	entry* header;
	entry* trailer;
	size_t hash_size;
	
};

#include "hash_map.cpp"  //Template definition must be visible to compiler
#include "iterator.cpp"

#endif

