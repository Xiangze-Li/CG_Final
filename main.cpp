#include "SPPM.hpp"
#include "Image.hpp"
#include "Ray.hpp"
#include "Scene.hpp"
#include <omp.h>
#include <cstdlib>
#include <cstdio>
#include <string>
#include <iostream>
#include <vector>

using std::cerr;
using std::cout;
using std::endl;

int main(int argc, char **argv)
{
    if (argc != 5)
    {
        cout << "Usage: \n\n"
             << "\t<EXCUTABLE> iter sample radius alpha\n"
             << endl;
        return 1;
    }
    int ITER = atoi(argv[1]);
    double SAMPLE = atof(argv[2]), RADIUS = atof(argv[3]),
           ALPHA = atof(argv[4]);

    const auto &camera = scene.camera();
    const auto &group = scene.group();
    const auto &light = scene.light();
    const auto &bgColor = scene.bg();

    int WIDTH = camera.width(), HEIGHT = camera.height();

    int thrNum = omp_get_num_procs();
    IMGbuffer **imgBuff = new IMGbuffer *[thrNum];
    for (size_t i = 0; i < thrNum; i++)
        imgBuff[i] = new IMGbuffer[WIDTH * HEIGHT];
    IMGbuffer *imgFinal = new IMGbuffer[WIDTH * HEIGHT];
    IMGbuffer *imgNow = new IMGbuffer[WIDTH * HEIGHT];

    std::vector<SPPMNode> ball[thrNum];
    for (int iter = 1; iter <= ITER; iter++)
    {
        KDtree tree;
        if (iter < 3 || iter % 1 == 0)
        {
            if (iter > 1)
            {
                SAMPLE /= sqrt(ALPHA);
                RADIUS *= ALPHA;
            }
#pragma omp parallel for num_thread(nth) schedule(dynamic)
            for (size_t y = 0; y < HEIGHT; y++)
            {
                int threadNo = omp_get_thread_num();
                cerr << "\rBuilding k-D tree " << 100. * double(y) / HEIGHT << "%. ";
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
            cerr << "\rBuilding k-D tree: Gathering threads ...";
            std::vector<SPPMNode> total;
            for (size_t i = 0; i < thrNum; i++)
                total.insert(total.end(), ball[i].begin(), ball[i].end());
            // KDtree tree(total);
            tree.init(total);
            cerr << "Done!" << endl;
        }

        int per = SAMPLE / thrNum + 1;
#pragma omp parallel for num_threads(nth) schedule(dynamic)
        for (size_t t = 0; t < thrNum; t++)
        {
            int threadNo = omp_get_thread_num();
            for (size_t photon = 0; photon < per; photon++)
            {
                if (threadNo == 0 && photon % 1000 == 0)
                    cerr << "\rSPPM tracing " << 100. * photon / per;

                auto l = light.generateRay();
                tree.query(SPPMNode(l.ori(), light.color(), l.dir()), imgBuff[threadNo]);
                sppmForward(&group, l, 0, light.color(), imgBuff[threadNo], &tree);
            }
        }

        memset(imgNow, 0, sizeof imgNow);
        for (size_t i = 0; i < thrNum; i++)
        {
            for (size_t j = HEIGHT * WIDTH - 1; j >= 0; j--)
                imgNow[j] += imgBuff[i][j];
            memset(imgBuff[i], 0, sizeof imgBuff[i]);
        }

        for (size_t i = HEIGHT * WIDTH - 1; i >= 0; i--)
            imgFinal[i] += imgNow[i] / imgNow[i].cntr;

        if (iter == 1 || iter % 1 == 0)
        {
            char sout[100];
            sprintf(sout, "%s%03d.ppm", scene.name().c_str(), iter);
            FILE *f = fopen(sout, "w");
            fprintf(f, "P6\n%d %d\n%d\n", WIDTH, HEIGHT, 255);
            for (int y = HEIGHT - 1; y >= 0; --y)
                for (int x = WIDTH - 1; x >= 0; --x)
                {
                    fprintf(f, "%c%c%c",
                            gamma_trans(imgFinal[y * WIDTH + x].getColor().x()),
                            gamma_trans(imgFinal[y * WIDTH + x].getColor().y()),
                            gamma_trans(imgFinal[y * WIDTH + x].getColor().z()));
                }
            fclose(f);
        }
        cerr << "\rIter #" << iter << " done." << endl;
    }
}
