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

SceneParser &scene = dinosaur2;

int main(int argc, char **argv)
{
    int EPOCH = 1;
    double SAMPLE = 200., RADIUS = 1.0, ALPHA = 0.5;

    if (argc != 1 && argc != 5)
    {
        cout << "Usage: \n\n"
             << "\t<EXCUTABLE> epoch sample radius alpha\n"
             << endl;
        return 1;
    }
    else if (argc == 5)
    {
        EPOCH = atoi(argv[1]);
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

    cerr << "Rendering \"" << scene.name() << "\". " << endl;

    int thrNum = omp_get_num_procs();
    cerr << "Using " << thrNum << " core(s)." << endl;

    IMGbuffer **imgBuff = new IMGbuffer *[thrNum];
    for (size_t i = 0; i < thrNum; i++)
        imgBuff[i] = new IMGbuffer[WIDTH * HEIGHT];
    IMGbuffer *imgFinal = new IMGbuffer[WIDTH * HEIGHT];
    IMGbuffer *imgNow = new IMGbuffer[WIDTH * HEIGHT];

    for (int epoch = 1; epoch <= EPOCH; epoch++)
    {
        cerr << "Begin rendering epoch #" << epoch << "/" << EPOCH << "." << endl;

        auto ball = new std::vector<SPPMNode>[thrNum];

        if (epoch > 1)
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
                cerr << "\rBuilding k-D tree " << std::fixed << std::setprecision(2) << 100. * double(y) / HEIGHT << "%. ";
                cerr.flush();
            }
            for (size_t x = 0; x < WIDTH; x++)
            {
                // 多重采样
                for (size_t subY = 0; subY < 3; subY++)
                    for (size_t subX = 0; subX < 3; subX++)
                    {
                        std::vector<SPPMNode> tmp = sppmBacktrace(&group, camera.generateRay(x + (subX - 1) / 2. + (rand01() - 0.5), y + (subY - 1) / 2. + (rand01() - 0.5)), 0, y * WIDTH + x);
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

        delete[] ball;

        cerr << "Spreading " << int(SAMPLE) << " photons in total." << endl;

        int per = SAMPLE / thrNum + 1;

#pragma omp parallel for num_threads(thrNum) schedule(dynamic, 1)
        for (size_t t = 0; t < thrNum; t++)
        {
            int threadNo = omp_get_thread_num();
            for (size_t photon = 0; photon < per; photon++)
            {
                if (threadNo == 0 && photon % 1000 == 0)
                {
                    cerr << "\rSPPM tracing " << std::setprecision(6) << 100. * photon / per << "%.";
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

        char sout[100];
        sprintf(sout, "output/%s__%03d.ppm", scene.name().c_str(), epoch);
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
        cerr << "Epoch #" << epoch << " done." << endl;
        cerr << "\n\n"
             << endl;
    }
}
