#include <vector>
#include <cmath>
#include "tgaimage.h"
#include "model.h"
#include "geometry.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);
Model *model = NULL;
const int width  = 800;
const int height = 800;

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
        } else {
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
    if (2 == argc) {
        model = new Model(argv[1]);
    } else {
        model = new Model("obj/african_head.obj");
    }

    TGAImage image(width, height, TGAImage::RGB);

    // Wireframe rendering
    // Draw lines read from coords in wavefront obj
    for (int i=0; i < model -> nfaces(); i++) {
        std::vector<int> face = model->face(i);
        for (int j=0; j < 3; j++) {
            Vec3f v0 = model -> vert(face[j]);
            Vec3f v1 = model -> vert(face[(j+1)%3]);
            int x0 = (v0.x+1.0) * width / 2.0;
            int y0 = (v0.y+1.0) * width / 2.0;
            int x1 = (v1.x+1.0) * width / 2.0;
            int y1 = (v1.y+1.0) * width / 2.0;
            line(x0, y0, x1, y1, image, white);
        }
    }

    image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image.write_tga_file("output.tga");
    return 0;
}
