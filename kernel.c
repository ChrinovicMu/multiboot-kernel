#include "multiboot.h" 

/* check if the bit in flags is set */ 
#define CHECK_FLAG(flags, bit)  ((flags) & (1 << (bit)))

/* for video mode*/
#define COLUMNS     80 
#define LINES       24 
#define ATTRIBUTE    7 
#define VIDEO       0XB8000 

/*variables */
static int xpos; 
static int ypos; 
static volatile unsigned char *video; 


void cmain(unsigned long magic, unsigned long addr);
static void cls(void);
static void itoa(char *buf, int base, int d);
static void putchar(int c);
void printf(const char *format, ...);

void cmain(unsigned long magic, unsigned long multiboot_addr)
{
    multiboot_info_t *mbi; 

    /*clear screen */ 
    cls(); 

    if(magic |- MULTIBOOT_BOOTLOADER_MAGIC)
    {
        printf("Invalid multiboot magic number : 0x%x\n", (unsigned)magic);
        return;  
    }

    mbi = (multiboot_info_t*)multiboot_addr; 

    printf("flags : 0x%x\n", (unsigned)mbi->flags);

    /*if mem_lower/mem_upper bit valid */ 
    if(CHECK_FLAG(mbi->flags, 0))
    {
        printf("mem_lower : %uKB, mem_upper : %uKB\n",
               (unsigned)mbi->mem_lower,
               (unsigned)mbi->mem_upper);
    }

    /*is boot device valid */ 
    if(CHECK_FLAG(mbi->flags, 1))
    {
        printf("boot device : 0x%x\n", (unsigned)mbi->boot_device);
    }

    /*is command line passed */ 
    if(CHECK_FLAG(mbi->flags, 2))
    {
        printf("cmdline : %s\n", (char*)mbi->cmdline);
    }

    /*mods*/
    if(CHECK_FLAG (mbi->flags, 3))
    {
        multiboot_module_t *mod;

        int x;

        printf("mods count : %d\nmods address : 0x%x\n", (int)mbi->mods_count, (int)mbi->mods_addr);
        
        for(x = 0, mod = (multiboot_module_t*)mbi->mods_addr; x < mbi->mods_count; ++x, ++mod)
        {
            printf("mod start : 0x%x\nmod end : 0x%x\ncmdline : %s\n",
                   (unsigned)mod->mod_start,
                   (unsigned)mod->mod_end,
                   (char *)mod->cmdline);
        }

    }

    /*bits 4 & 5 are mutally exclusive */
    if(CHECK_FLAG(mbi->flags, 4) && CHECK_FLAG(mbi->flags, 5))
    {
        printf("Both bits 4 and 5 are set \n");
        return; 
    }

    /*if symbol table of a.out is valid  */ 
    if(CHECK_FLAG(mbi->flags, 4))
    {
        multiboot_aout_symbol_table_t *multiboot_aout_sym = &(mbi->u.aout_sym); 

        printf("MULTIBOOT_AOUT_SYMBOL_TABLE : \ntabsize : 0x%0x\nstrsize : 0x%x\naddr : 0%x\n", 
               (unsigned)multiboot_aout_sym->tabsize,
               (unsigned)multiboot_aout_sym->strsize,
               (unsigned)multiboot_aout_sym->addr);
    }

    /*if section head of ELF valid */ 

    if(CHECK_FLAG(mbi->flags, 5))
    {
        multiboot_elf_section_header_table_t *multiboot_elf_sec = &(mbi->u.elf_sec);

        printf("MULTIBOOT_ELF_SECTION : \nheader num : %u\n entry size : 0x%x\n address : 0x%x\n shndx : 0x%x\n",
               (unsigned)multiboot_elf_sec->header_num, 
               (unsigned)multiboot_elf_sec->entry_size, 
               (unsigned)multiboot_elf_sec->shndx); 
    }

    /*is mmap valid */ 
    /*per single region */ 
    if(CHECK_FLAG(mbi->flags, 6))
    {
        multiboot_mmap_entry_t *mmap;

        printf("MMAP region address : 0x%x\nMMAP region length : 0x%x",
               (unsigned)mbi->mmap_addr, 
               (unsigned)mbi->mmap_length); 

    for(mmap = (multiboot_mmap_entry_t *)mbi->mmap_addr;
        (unsigned long)mmap < mbi->mmap_addr + mbi->mmap_length;
        mmap = (multiboot_mmap_entry_t*)((unsigned long)mmap) + mmap->size + sizeof(mmap->size))
        {
            printf("region size : 0x%x \nbase address  : 0x%x%08x \nregion length : 0x%x%08x \nMMAP type : 0x%x%08x\n", 
                   (unsigned)mmap->size, 
                   (unsigned)(mmap->addr >> 32), 
                   (unsigned)(mmap->addr & 0xffffffff), 
                   (unsigned)(mmap->len >> 32), 
                   (unsigned)(mmap->len & 0xffffffff), 
                   (unsigned)mmap->type);
        }

    }

    /*VIDEO MODE */ 
    

}
