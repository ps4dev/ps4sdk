#include <stddef.h>

#define PS4_KERN_ELF_RANGE_ADDRESS (char *)0xffffffff80000000
enum{ PS4_KERN_ELF_RANGE_SIZE = 0x02000000 };
#define PS4_KERN_ELF_FIXED_ADDRESS (char *)0xffffffff80700000
//enum{ PS4_KERNEL_ELF_SIZE = 0x00EAC180 };
enum{ PS4_KERN_ELF_PAGE_SIZE = 16 * 1024 }; // no dependencies

void *ps4KernSeekElfAddress()
{
	char *m;
	int i;
	unsigned char elfMagic[] = {0x7f, 'E', 'L', 'F', 0x02, 0x01, 0x01, 0x09, 0x00};
	const size_t magicSize = sizeof(elfMagic) / sizeof(*elfMagic);

	m = PS4_KERN_ELF_FIXED_ADDRESS;
	for(i = 0; i < magicSize && m[i] == elfMagic[i]; ++i);
	if(i == magicSize)
		return m;

	for(m = PS4_KERN_ELF_RANGE_ADDRESS;
		m < PS4_KERN_ELF_RANGE_ADDRESS + PS4_KERN_ELF_RANGE_SIZE;
		m += PS4_KERN_ELF_PAGE_SIZE)
	{
		for(i = 0; i < magicSize && m[i] == elfMagic[i]; ++i);
		if(i == magicSize)
			return m;
	}

	return NULL;
}
