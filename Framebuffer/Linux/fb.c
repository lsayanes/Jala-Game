#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#define FB_DEVICE "/dev/fb0"

int main()
{
    int fbfd = 0;
    unsigned short int* fbp = 0;
    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;
    long int screensize = 0;
    int x = 0, y = 0;
    long int location = 0;

    // Open the frame buffer device
    fbfd = open(FB_DEVICE, O_RDWR);
    if (fbfd == -1) {
        printf("Error: cannot open frame buffer device.\n");
        return 1;
    }

    // Get fixed screen information
    if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo) == -1) {
        printf("Error reading fixed information.\n");
        return 2;
    }

    // Get variable screen information
    if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo) == -1) {
        printf("Error reading variable information.\n");
        return 3;
    }

    printf("Display info:\n");
    printf("  Resolution: %dx%d, %dbpp\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel);
    printf("  Virtual resolution: %dx%d\n", vinfo.xres_virtual, vinfo.yres_virtual);
    printf("  Offset: %dx%d\n", vinfo.xoffset, vinfo.yoffset);

    // Map frame buffer to user memory
    screensize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;
    fbp = (unsigned short int*)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
    if ((int)fbp == -1) {
        printf("Failed to mmap.\n");
        return 4;
    }

    // Fill the screen with a solid color
    for (y = 0; y < vinfo.yres; y++) {
        for (x = 0; x < vinfo.xres; x++) {
            location = x + vinfo.xoffset + (y + vinfo.yoffset) * vinfo.xres_virtual;
            *(fbp + location) = 0xFFFF; // White color
        }
    }

    munmap(fbp, screensize);
    close(fbfd);
    return 0;
}
