#include "PageDirectoryEntry.h"

namespace PageDirectoryEntry
{
	void AddAttribute(PDE* entry, uint32_t attr)
	{
		*entry |= attr;
	}

	void DelAttribute(PDE* entry, uint32_t attr)
	{
		*entry &= ~attr;
	}

	void SetFrame(PDE* entry, uint32_t addr)
	{
		*entry = (*entry & ~I86_PDE_FRAME) | addr;
	}

	bool IsPresent(PDE entry)
	{
		return entry & I86_PDE_PRESENT;
	}

	bool IsWritable(PDE entry)
	{
		return (entry & I86_PDE_WRITABLE) > 0;
	}

	uint32_t GetFrame(PDE entry)
	{
		return entry & I86_PDE_FRAME;
	}

	bool IsUser(PDE entry)
	{
		return (entry & I86_PDE_USER) > 0;
	}
	 
	bool Is4mb(PDE entry)
	{
		return (entry & I86_PDE_4MB) > 0;
	}
};