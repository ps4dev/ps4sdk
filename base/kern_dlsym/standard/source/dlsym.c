#include <stddef.h>
#include <elf.h>

#include <ps4/kern.h>

static Elf64_Sym *ps4KernDlSymElfSymbols;
static char *ps4KernDlSymElfStrings;

static void ps4KernDlSymInitialize(void)
{
	void *kernelAddress = ps4KernSeekElfAddress();
	if(kernelAddress == NULL)
		return;

	// Ah, old-school/kernel C naming conventions ... terrible stuff
	Elf64_Ehdr *ehdr = (Elf64_Ehdr *)kernelAddress;
	/*for(size_t i = 0; i < ehdr->e_phnum; i++)
	{
		Elf64_Phdr *phdr = (Elf64_Phdr *)(kernelAddress + ehdr->e_phoff) + i;
		if (phdr->p_type == PT_PHDR)
			ehdr = (Elf64_Ehdr *)(phdr->p_vaddr - ehdr->e_phoff);
	}*/
	Elf64_Phdr *phdr = (Elf64_Phdr *)((uint8_t *)ehdr + ehdr->e_phoff);
	for(size_t i = 0; i < ehdr->e_phnum; i++, phdr++)
		if (phdr->p_type == PT_DYNAMIC)
		{
			Elf64_Dyn *dyn = (Elf64_Dyn *)phdr->p_vaddr;
			for (Elf64_Dyn *d = dyn; d->d_tag != DT_NULL; d++)
				switch (d->d_tag)
				{
					case DT_SYMTAB:
						ps4KernDlSymElfSymbols = (Elf64_Sym *)d->d_un.d_ptr;
						break;
					case DT_STRTAB:
						ps4KernDlSymElfStrings = (char *)d->d_un.d_ptr;
						break;
				}
		}
}

void *ps4KernDlSym(char *name)
{
	Elf64_Sym *symbol;

	if(ps4KernDlSymElfSymbols == NULL || ps4KernDlSymElfStrings == NULL)
		ps4KernDlSymInitialize();

	for(symbol = ps4KernDlSymElfSymbols; symbol + 1 < (Elf64_Sym *)ps4KernDlSymElfStrings; ++symbol)
	{
		int j;
		char *n = (char *)&ps4KernDlSymElfStrings[symbol->st_name];
		for(j = 0; n[j] == name[j] && n[j] != 0; ++j);
		if(j > 0 && n[j] == '\0' && name[j] == '\0')
			return (void *)symbol->st_value;
	}

	return NULL;
}
