#include <vector>
#include <cmath>
#include "tgaimage.h"
#include "model.h"
#include "geometry.h"

const TGAColor red      = TGAColor(255, 0,   0,   255);
const TGAColor white    = TGAColor(255, 255, 255, 255);
const TGAColor green    = TGAColor(0,   255, 0,   255);
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

void triangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage &image, TGAColor color) {
    // Bubble sort verticies by their y coords'
    if (t0.y > t1.y) std::swap(t0, t1);
    if (t1.y > t2.y) std::swap(t1, t2);
    if (t0.y > t1.y) std::swap(t0, t1);

    int total_height = t2.y-t0.y;

    // Fill the triangle
    for (int i=0; i<total_height; i++) {
        // Conditionals allow for both halfs of the triangle
        bool second_half = i > t1.y - t0.y || t1.y == t0.y;
        int segment_height = second_half ? t2.y-t1.y : t1.y-t0.y;

        // Get the position of the left and right edge of the triangle segment
        float alpha = (float) i / total_height;
        float beta  = (float) (i - (second_half ? t1.y-t0.y : 0)) / segment_height;
        Vec2i A =               t0 + (t2-t0)*alpha;
        Vec2i B = second_half ? t1 + (t2-t1)*beta : t0 + (t1-t0)*beta;
        if (A.x>B.x) std::swap(A, B);

        // Draw pixels connecting the left and right edge of the triangle
        for (int j=A.x; j<=B.x; j++) {
            image.set(j, t0.y+i, color);
        }
    }

    line(t0.x, t0.y, t1.x, t1.y, image, red);
    line(t1.x, t1.y, t2.x, t2.y, image, white);
    line(t2.x, t2.y, t0.x, t0.y, image, green);
}

int main(int argc, char** argv) {
    if (2 == argc) {
        model = new Model(argv[1]);
    } else {
        model = new Model("obj/african_head.obj");
    }

    TGAImage image(width, height, TGAImage::RGB);

    triangle(Vec2i(100, 700), Vec2i(200, 400), Vec2i(50, 70), image, white);

    // // Wireframe rendering
    // // Draw lines read from coords in wavefront obj
    // for (int i=0; i < model -> nfaces(); i++) {
    //     std::vector<int> face = model->face(i);
    //     for (int j=0; j < 3; j++) {
    //         Vec3f v0 = model -> vert(face[j]);
    //         Vec3f v1 = model -> vert(face[(j+1)%3]);
    //         int x0 = (v0.x+1.0) * width / 2.0;
    //         int y0 = (v0.y+1.0) * width / 2.0;
    //         int x1 = (v1.x+1.0) * width / 2.0;
    //         int y1 = (v1.y+1.0) * width / 2.0;
    //         line(x0, y0, x1, y1, image, white);
    //     }
    // }

    image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image.write_tga_file("output.tga");
    return 0;
}
