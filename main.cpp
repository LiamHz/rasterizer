#include <vector>
#include <cmath>
#include "tgaimage.h"
#include "model.h"
#include "geometry.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);
const TGAColor green = TGAColor(0,   255, 0,   255);

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
    // Ignore deggenerate triangles
    if (t0.y == t1.y && t0.y == t2.y) return;

    // Bubble sort the verticies by y coord ascending
    if (t0.y > t1.y) std::swap(t0, t1);
    if (t0.y > t2.y) std::swap(t0, t2);
    if (t1.y > t2.y) std::swap(t1, t2);

    int total_height = t2.y - t0.y;

    for (int y=0; y<total_height; y++) {
        // Use conditionals to accound for both triangle segments
        bool second_half = y > t1.y - t0.y || t1.y == t0.y;
        int segment_height = second_half ? t2.y - t1.y : t1.y - t0.y;

        // Get the position of the left and right edge of the triangle at ypos
        float alpha = (float) y / total_height;
        float beta  = (float)(y - (second_half ? t1.y - t0.y : 0))/segment_height;
        Vec2i left =                t0 + (t2 - t0) * alpha;
        Vec2i right = second_half ? t1 + (t2 - t1) * beta : t0 + (t1 - t0) * beta;
        if (left.x > right.x) std::swap(left, right);

        // Line fill from left to right edge
        for (int x=left.x; x <= right.x; x++) {
            image.set(x, t0.y+y, color); // attention, due to int casts t0.y+i != A.y
        }
    }
}

int main(int argc, char** argv) {
    if (2==argc) {
        model = new Model(argv[1]);
    } else {
        model = new Model("obj/african_head.obj");
    }

    TGAImage image(width, height, TGAImage::RGB);
    Vec3f light_dir(0,0,-1);

    // Model rendering
    // Draw lines read from coords in wavefront obj
    for (int i=0; i<model->nfaces(); i++) {
        std::vector<int> face = model->face(i);
        Vec2i screen_coords[3];
        Vec3f world_coords[3];
        for (int j=0; j<3; j++) {
            Vec3f v = model->vert(face[j]);
            screen_coords[j] = Vec2i((v.x+1.)*width/2., (v.y+1.)*height/2.);
            world_coords[j]  = v;
        }
        Vec3f n = (world_coords[2]-world_coords[0])^(world_coords[1]-world_coords[0]);
        n.normalize();
        float intensity = n*light_dir;
        if (intensity>0) {
            triangle(screen_coords[0], screen_coords[1], screen_coords[2], image, TGAColor(intensity*255, intensity*255, intensity*255, 255));
        }
    }

    image.flip_vertically();
    image.write_tga_file("output.tga");
    delete model;
    return 0;
}
