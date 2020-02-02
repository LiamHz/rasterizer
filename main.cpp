#include <cmath>
#include "tgaimage.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);

void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color) {
    bool steep = false;

    // If the line's height is greater than its width, transpose the image
    // This allows for the line drawing algorithm to work for all lines
    if (std::abs(x0-x1) < std::abs(y0-y1)) {
        std::swap(x0, y0);
        std::swap(x1, y1);
        steep = true;
    }

    // Ensure x0 is to the left of x1
    if (x0 > x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }

    int dx = x1 - x0;
    int dy = y1 - y0;
    float derror2 = std::abs(dy)*2;
    float error2 = 0;
    int y = y0;

    for (int x = x0; x <= x1; x++) {
        if (steep) {
            // If transposed, de-transpose
            image.set(y, x, color);
        }
        else {
            image.set(x, y, color);
        }

        // Adjust y if the y-coord deviation from line is significant
        error2 += derror2;
        if (error2 > dx) {
            y += (y1 > y0 ? 1 : -1);
            error2 -= dx*2;
        }
    }
}

int main(int argc, char** argv) {
    TGAImage image(100, 100, TGAImage::RGB);
    line(20, 13, 40, 80, image, red);
    line(80, 40, 13, 20, image, red);
    image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image.write_tga_file("output.tga");
    return 0;
}
