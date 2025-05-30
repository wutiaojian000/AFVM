#include <stdio.h>
#include <unistd.h>
#include <elf.h>
#include <sys/ioctl.h>
#include <fcntl.h>

int load_elf(const char *filename, const char *ram, unsigned int ram_len)
{
    int fd = open(filename, O_RDONLY);
    int magic = 0x464c457f;
    int i;
    unsigned int offset;
    Elf32_Ehdr ehdr;//elf header
    Elf32_Phdr phdr;//elf header
    if(fd < 0)
    {
        perror("error open file.\n");
        goto label1;
    }

    if(read(fd, &ehdr, sizeof(ehdr)) != sizeof(ehdr))
    {
        perror("error read elf header.\n");
        goto label2;
    }

    if(memcmp(ehdr, magic, 4) != 0)
    {
        perror("not an elf file.\n");
        goto label2;
    }

    lseek(fd, ehdr.e_phoff, SEEK_SET);
    offset = ehdr.e_phoff;
    for(i = 0; i < ehdr.e_phnum; ++i)
    {
        if(read(fd, &phdr, sizeof(phdr)) != sizeof(phdr))
        {
            perror("error format.\n");
            goto label2;
        }

        if(phdr.p_type == PT_LOAD)
        {
            if((phdr.p_paddr >= ram_len) || (phdr.p_paddr + phdr.p_filesz > ram_len))
            {
                perror("error format.\n");
                goto label2;
            }

            lseek(fd, phdr.p_offset, SEEK_SET);printf("offset:%x\n", phdr.p_offset);
            if(read(fd, ram + phdr.p_paddr, phdr.p_filesz) != phdr.p_filesz)
            {
                perror("error copy from elf.\n");
                goto label2;
            }

            lseek(fd, offset + sizeof(phdr), SEEK_SET);
            offset += sizeof(phdr);
        }
    }

    return 1;

label2:
    close(fd);
label1:
    return -1;

}