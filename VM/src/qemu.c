#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/kvm.h>

#include "extractELF.h"

#define RAM_SIZE (0x100000)

int main() {
    int kvm_fd = -1;
    int vm_fd = -1;
    int vcpu_fd = -1;
    int kfd = -1;
    unsigned char *ram = MAP_FAILED;
    struct kvm_run *run = MAP_FAILED;

    // 打开 KVM 设备
    kvm_fd = open("/dev/kvm", O_RDWR);
    if (kvm_fd < 0) {
        perror("open /dev/kvm");
        goto out;
    }

    // 获取 API 版本
    int api_version = ioctl(kvm_fd, KVM_GET_API_VERSION, NULL);
    if (api_version != KVM_API_VERSION) {
        fprintf(stderr, "KVM API version mismatch: expected %d, got %d\n",
                KVM_API_VERSION, api_version);
        goto out;
    }

    // 创建虚拟机
    vm_fd = ioctl(kvm_fd, KVM_CREATE_VM, (unsigned long)0);
    if (vm_fd < 0) {
        perror("KVM_CREATE_VM");
        goto out;
    }

    // 分配内存映射区域
    ram = mmap(NULL, RAM_SIZE,
               PROT_READ | PROT_WRITE,
               MAP_SHARED | MAP_ANONYMOUS,
               -1, 0);
    if (ram == MAP_FAILED) {
        perror("mmap");
        goto out;
    }

    // 清空内存
    memset(ram, 0, RAM_SIZE);

    // 加载二进制文件到内存
    // kfd = open("./main.1.0.0", O_RDONLY);
    // if (kfd < 0) {
    //     perror("open ./test.bin");
    //     goto out;
    // }

    //解析elf格式并加载到内存镜像正确位置


    // ssize_t bytes_read = read(kfd, ram, RAM_SIZE);
    // if (bytes_read < 0) {
    //     perror("read from test.bin");
    //     goto out;
    // }
    if(load_elf("./main.1.0.0", ram, RAM_SIZE) < 0)
    {
        perror("read from test.bin");
        goto out;
    }

    //printf("Loaded %ld bytes into guest memory\n", bytes_read);

    // 设置用户空间内存区域
    struct kvm_userspace_memory_region mem = {
        .slot = 0,
        .guest_phys_addr = 0,
        .memory_size = RAM_SIZE,
        .userspace_addr = (unsigned long)ram,
    };

    if (ioctl(vm_fd, KVM_SET_USER_MEMORY_REGION, &mem) < 0) {
        perror("KVM_SET_USER_MEMORY_REGION");
        goto out;
    }

    // 创建 VCPU
    vcpu_fd = ioctl(vm_fd, KVM_CREATE_VCPU, 0);
    if (vcpu_fd < 0) {
        perror("KVM_CREATE_VCPU");
        goto out;
    }

    // 获取 VCPU 内存大小
    int mmap_size = ioctl(kvm_fd, KVM_GET_VCPU_MMAP_SIZE, NULL);
    if (mmap_size < 0) {
        perror("KVM_GET_VCPU_MMAP_SIZE");
        goto out;
    }

    run = mmap(NULL, mmap_size, PROT_READ | PROT_WRITE, MAP_SHARED, vcpu_fd, 0);
    if (run == MAP_FAILED) {
        perror("mmap for vcpu run");
        goto out;
    }

    // 初始化段寄存器
    struct kvm_sregs sregs;
    if (ioctl(vcpu_fd, KVM_GET_SREGS, &sregs) < 0) {
        perror("KVM_GET_SREGS");
        goto out;
    }

    // 设置 CS 段寄存器（简单模式）
    sregs.cs.base = 0;
    sregs.cs.selector = 0;

    if (ioctl(vcpu_fd, KVM_SET_SREGS, &sregs) < 0) {
        perror("KVM_SET_SREGS");
        goto out;
    }

    // 设置 RIP（指令指针）
    struct kvm_regs regs;
    memset(&regs, 0, sizeof(regs));
    regs.rip = 0x1000; // 从内存地址 0 开始执行

    if (ioctl(vcpu_fd, KVM_SET_REGS, &regs) < 0) {
        perror("KVM_SET_REGS");
        goto out;
    }

    // 运行虚拟机
    while (1) {
        if (ioctl(vcpu_fd, KVM_RUN, 0) < 0) {
            perror("KVM_RUN");
            break;
        }

        switch (((struct kvm_run *)run)->exit_reason) {
            case KVM_EXIT_HLT:
                printf("KVM_EXIT_HLT\n");
                goto out;
            case KVM_EXIT_IO:
		printf("%c", *(((char*)run) + run->io.data_offset));
                //printf("KVM_EXIT_IO\n");
                break;
            case KVM_EXIT_FAIL_ENTRY:
                printf("KVM_EXIT_FAIL_ENTRY\n");
                goto out;
            default:
                printf("Unknown exit reason: %u\n",
                       ((struct kvm_run *)run)->exit_reason);
                goto out;
        }
    }

out:
    // 资源清理
    if (run != MAP_FAILED) {
        int mmap_size = ioctl(kvm_fd, KVM_GET_VCPU_MMAP_SIZE, NULL);
        if (mmap_size > 0)
            munmap(run, mmap_size);
    }

    if (vcpu_fd >= 0)
        close(vcpu_fd);

    if (ram != MAP_FAILED)
        munmap(ram, RAM_SIZE);

    if (kfd >= 0)
        close(kfd);

    if (vm_fd >= 0)
        close(vm_fd);

    if (kvm_fd >= 0)
        close(kvm_fd);

    return 0;
}
