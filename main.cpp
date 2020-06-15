#define _CRT_SECURE_NO_WARNINGS

#include "SPPM.hpp"
#include "Image.hpp"
#include "Ray.hpp"
#include "Scene.hpp"
#include <cstdlib>
#include <cstdio>
#include <string>
#include <iostream>
#include <iomanip>
#include <vector>

#define OMP_GET_NUM_THREADS
#define OMP_GET_THREAD_NUM
#include "omp.h"

using std::cerr;
using std::cout;
using std::endl;

/*int main()
{
    Hit hit;
    auto& group = scene.group();
    auto ray = scene.camera().generateRay(320, 240);
    bool result = group.intersect(ray, hit);
    return 0;
}*/

int main(int argc, char **argv)
{
    int ITER = 1;
    double SAMPLE = 1200., RADIUS = 2., ALPHA = 0.8;
    if (argc != 1 && argc != 5)
    {
        cout << "Usage: \n\n"
             << "\t<EXCUTABLE> iter sample radius alpha\n"
             << endl;
        return 1;
    }
    else if (argc == 5)
    {
        ITER = atoi(argv[1]);
        SAMPLE = atof(argv[2]);
        RADIUS = atof(argv[3]);
        ALPHA = atof(argv[4]);
    }

    const auto &camera = scene.camera();
    const auto &group = scene.group();
    const auto &light = scene.light();
    const auto &bgColor = scene.bg();

    int WIDTH = camera.width(), HEIGHT = camera.height();
    SAMPLE *= WIDTH * HEIGHT;

    int thrNum = omp_get_num_procs();
    cerr << "Using " << thrNum << " core(s)." << endl;

    IMGbuffer **imgBuff = new IMGbuffer *[thrNum];
    for (size_t i = 0; i < thrNum; i++)
        imgBuff[i] = new IMGbuffer[WIDTH * HEIGHT];
    IMGbuffer *imgFinal = new IMGbuffer[WIDTH * HEIGHT];
    IMGbuffer *imgNow = new IMGbuffer[WIDTH * HEIGHT];

    auto *ball = new std::vector<SPPMNode>[thrNum];
    for (int iter = 1; iter <= ITER; iter++)
    {
        //KDtree tree;
        // if (iter < 3 || iter % 1 == 0)
        // {
        if (iter > 1)
        {
            SAMPLE /= sqrt(ALPHA);
            RADIUS *= ALPHA;
        }

#pragma omp parallel for num_threads(thrNum) schedule(dynamic)
        for (size_t y = 0; y < HEIGHT; y++)
        {
            int threadNo = omp_get_thread_num();
            if (threadNo == 0)
            {
                cerr << "\rBuilding k-D tree " << std::fixed << std::setprecision(2) <<  100. * double(y) / HEIGHT << "%. ";
                cerr.flush();
            }
            for (size_t x = 0; x < WIDTH; x++)
            {
                for (size_t subY = 0; subY < 2; subY++)
                    for (size_t subX = 0; subX < 2; subX++)
                    {
                        // double r1 = 2 * rand01(), dx = r1 < 1 ? sqrt(r1) : 2 - sqrt(r1);
                        // double r2 = 2 * rand01(), dy = r2 < 1 ? sqrt(r2) : 2 - sqrt(r2);
                        // Vec3 dir = camX * ((dx / 2. + x + subX) / WIDTH - 0.5) +
                        //            camY * ((dy / 2. + y + subY) / HEIGHT - 0.5) + camera.dir();
                        // Vec3 pp = camera.ori() + dir * 150., loc = camera.ori() + (Vec3(rand01() * 1.05, rand01() - .5, 0.) * 2 * APERTURE);

                        std::vector<SPPMNode> tmp = sppmBacktrace(&group, camera.generateRay(x + subX, y + subY), 0, y * WIDTH + x);
                        for (auto &nn : tmp)
                            if (nn.index > 0)
                            {
                                nn.r = RADIUS;
                                ball[threadNo].push_back(nn);
                            }
                    }
            }
        }
        cerr << "\nBuilding k-D tree: Gathering threads ...";
        cerr.flush();
        std::vector<SPPMNode> total;
        for (size_t i = 0; i < thrNum; i++)
            total.insert(total.end(), ball[i].begin(), ball[i].end());
        KDtree tree(total);
        //tree.init(total);
        cerr << "Done!" << endl;
        cerr << "Built a k-D tree with " << total.size() << " points. " << endl;
        // }

        delete[] ball;

        int per = SAMPLE / thrNum + 1;

#pragma omp parallel for num_threads(thrNum) schedule(dynamic, 1)
        for (size_t t = 0; t < thrNum; t++)
        {
            int threadNo = omp_get_thread_num();
            for (size_t photon = 0; photon < per; photon++)
            {
                if (threadNo == 0 && photon % 1000 == 0)
                {
                    cerr << "\rSPPM tracing " << 100. * photon / per << "%.";
                    cerr.flush();
                }
                auto l = light.generateRay();
                tree.query(SPPMNode(l.ori(), light.color(), l.dir()), imgBuff[threadNo]);
                sppmForward(&group, l, 0, light.color(), imgBuff[threadNo], &tree);
            }
            if (threadNo == 0)
                cerr << endl;
        }
        cerr << "SPPM tracing ended!" << endl;

        for (int j = HEIGHT * WIDTH - 1; j >= 0; j--)
            imgNow[j].reset();

        for (size_t i = 0; i < thrNum; i++)
        {
            for (int j = HEIGHT * WIDTH - 1; j >= 0; j--)
            {
                imgNow[j] += imgBuff[i][j];
                imgBuff[i][j].reset();
            }
        }

        for (int i = HEIGHT * WIDTH - 1; i >= 0; i--)
            imgFinal[i] += imgNow[i] / imgNow[i].cntr;

        if (iter == 1 || iter % 1 == 0)
        {
            char sout[100];
            sprintf(sout, "%s%03d.ppm", scene.name().c_str(), iter);
            FILE *f = fopen(sout, "w");
            fprintf(f, "P3\n%d %d\n%d\n", WIDTH, HEIGHT, 255);
            for (int y = HEIGHT - 1; y >= 0; --y)
                for (int x = WIDTH - 1; x >= 0; --x)
                {
                    fprintf(f, "%d %d %d\n",
                            gamma_trans(imgFinal[y * WIDTH + x].getColor().x()),
                            gamma_trans(imgFinal[y * WIDTH + x].getColor().y()),
                            gamma_trans(imgFinal[y * WIDTH + x].getColor().z()));
                }
            fclose(f);
        }
        cerr << "Iter #" << iter << " done." << endl;
    }
}
