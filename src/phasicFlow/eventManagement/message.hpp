/*------------------------------- phasicFlow ---------------------------------
      O        C enter of
     O O       E ngineering and
    O   O      M ultiscale modeling of
   OOOOOOO     F luid flow       
------------------------------------------------------------------------------
  Copyright (C): www.cemf.ir
  email: hamid.r.norouzi AT gmail.com
------------------------------------------------------------------------------  
Licence:
  This file is part of phasicFlow code. It is a free software for simulating 
  granular and multiphase flows. You can redistribute it and/or modify it under
  the terms of GNU General Public License v3 or any other later versions. 
 
  phasicFlow is distributed to help others in their research in the field of 
  granular and multiphase flows, but WITHOUT ANY WARRANTY; without even the
  implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

-----------------------------------------------------------------------------*/
#ifndef __message_hpp__
#define __message_hpp__

#include <bitset>
#include <array>

#include "types.hpp"
#include "iOstream.hpp"
#include "List.hpp"


namespace pFlow
{

class message
{
public:
	enum EVENT : size_t
	{
		RANGE_CHANGED	  = 0,  // internal points active range has been changed
		ITEMS_INSERT  	  = 1,  // internal points are being inserted 
		ITEMS_DELETE	  = 2,  // internal points are being deleted
		ITEMS_REARRANGE   = 3,  // internal points are being rearrange
		ITEMS_FLAGCHANGED = 4,  // internal points flag changed, this occurs when transfer occurs
		BNDR_APPEND		  = 5,  // boundary: new indices are appended  
		BNDR_RESET		  = 6,  // boundary: indices are reset entirely  
		BNDR_DELETE 	  = 7,  // boundary: indices are deleted
		BNDR_PROC_SIZE_CHANGED = 8,      // processor boundary: size has been changed
		BNDR_PROCTRANSFER_SEND = 9,      // processor boundary: transfer of data between processors step 1 (send) 
		BNDR_PROCTRANSFER_RECIEVE = 10,  // processor boundary: transfer of data between processors step 2 (recieve)
		BNDR_PROCTRANSFER_WAITFILL = 11  // processor boundary: wait for data and fill the field 
	};

	
private:

	static constexpr size_t numberOfEvents_ = 12;

	std::bitset<numberOfEvents_> events_{0x0000};
	
	static
	inline const std::array<word,numberOfEvents_> eventNames_
	{
		"activeRange",
		"insertedIndices",
		"deletedIndices",
		"rearrangedIndices",
		"flagChange",
		"bndryAppendedIndices",
		"bndryReset",
		"bndryDeletedIndices",
		"bndrySize",
		"bndryTransferIndices",
		"bndryNumToRecieve",
		"bndryWait"
	};

public:

	message()=default;

	message(EVENT evnt)
	{
		add(evnt);
	}

	message(size_t i )
	{
		if(i<numberOfEvents_)
		{
			events_.set(i);
		}
	}

	message(const message&) = default;

	message(message&&) = default;

	message& operator=(const message&) = default;

	message& operator=(message&&) = default;

	~message()=default;

	inline message& add( EVENT evnt)
	{
		events_.set(static_cast<size_t>(evnt));
		return *this;
	}

	inline const word& addAndName(EVENT evnt)
	{
		add(evnt);
		return eventName(static_cast<size_t>(evnt));
	}

	inline message& remove(EVENT evnt)
	{
		events_.set(static_cast<size_t>(evnt), false);
		return *this;
	}

	inline bool equivalentTo( EVENT evnt )const
	{
		return equivalentTo(static_cast<size_t>(evnt));
	}

	inline bool equivalentTo(size_t i)const
	{
		return events_.test(i);
	}

	inline bool isNull()const
	{
		return events_.none();
	}

	inline auto size()const
	{
		return events_.size();
	}

	inline
	message& operator+(EVENT evnt)
	{
		return add(evnt);
	}

	inline
	message& operator-(EVENT evnt)
	{
		return remove(evnt);
	}

	inline 
	message& operator+(const message& msg)
	{
		for(size_t i=0uL; i< events_.size(); i++)
		{
			if(msg.equivalentTo(i))
			{
				events_.set(i);
			}
		}
		return *this;
	}
	
	wordList eventNames()const
	{
		wordList lst;
		for(size_t i=0; i<events_.size(); i++)
		{
			if(events_[i])
				lst.push_back(eventNames_[i]);
		}
		return lst;
	}

	static
	auto constexpr numEvents()
	{
		return numberOfEvents_;
	}
	
	static 
	message Empty()	
	{
		message msg;
		return msg;
	}

	static
	const word& eventName(size_t event)
	{
		return eventNames_[event];
	}
	
};

inline
iOstream& operator<<(iOstream& os, const message& msg)
{
	for(size_t i=msg.size(); i>0; i--)
	{
		os<< msg.equivalentTo(i-1)? '1':0;
	}
	os<<endl;

	return os;
}

inline
message operator + (message::EVENT evnt1, message::EVENT evnt2)
{
	message msg = message::Empty().add(evnt1).add(evnt2);
	return msg;
}

}

#endif // __eventMessage_hpp__
