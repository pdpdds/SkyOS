#include "PageTableEntry.h"

namespace PageTableEntry
{
	void AddAttribute(PTE* entry, uint32_t attr)
	{
		*entry |= attr;
	}

	void DelAttribute(PTE* entry, uint32_t attr)
	{
		*entry &= ~attr;
	}

	void SetFrame(PTE* entry, uint32_t addr)
	{
		*entry = (*entry & ~I86_PTE_FRAME) | addr;
	}

	bool IsPresent(PTE entry)
	{
		return entry & I86_PTE_PRESENT;
	}

	bool IsWritable(PTE entry)
	{
		return (entry & I86_PTE_WRITABLE) > 0;
	}

	uint32_t GetFrame(PTE entry)
	{
		return entry & I86_PTE_FRAME;
	}
};