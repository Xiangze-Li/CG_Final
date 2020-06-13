#include "SPPM.hpp"
#include "Image.hpp"
#include "Ray.hpp"
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
    if (argc != 7)
    {
        cout << "Usage: \n\n"
             << "\t<EXCUTABLE> filename iter sample radius alpha aperture\n"
             << endl;
    }
    std::string FILENAME = argv[1];
    int ITER = atoi(argv[2]);
    double SAMPLE = atof(argv[3]), RADIUS = atof(argv[4]),
           ALPHA = atof(argv[5]), APERTURE = atof(argv[6]);

    int WIDTH, HEIGHT;
    Object *scene = nullptr;
    // TODO: SceneParser shoule behave like below
    // SceneParser sp(FILENAME);
    // Object *scene = sp.parse(WIDTH, HEIGHT);

    int nth = omp_get_num_procs();
    IMGbuffer **imgBuff = new IMGbuffer *[nth];
    for (size_t i = 0; i < nth; i++)
        imgBuff[i] = new IMGbuffer[WIDTH * HEIGHT];
    IMGbuffer *imgFinal = new IMGbuffer[WIDTH * HEIGHT];
    IMGbuffer *imgNow = new IMGbuffer[WIDTH * HEIGHT];

    Ray camera = Ray(Vec3(150., 28., 260.), Vec3(-0.45, 0.001, -1.).normalized());
    Vec3 camX(WIDTH * .33 / HEIGHT, 0., 0.), camY(camX.cross(Vec3(camera.dir().x(), 0., camera.dir().z())).normalized() * 0.33);
    camX *= 1.05;

    std::vector<SPPMNode> ball[nth];
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
                            double r1 = 2 * rand01(), dx = r1 < 1 ? sqrt(r1) : 2 - sqrt(r1);
                            double r2 = 2 * rand01(), dy = r2 < 1 ? sqrt(r2) : 2 - sqrt(r2);
                            Vec3 dir = camX * ((dx / 2. + x + subX) / WIDTH - 0.5) +
                                       camY * ((dy / 2. + y + subY) / HEIGHT - 0.5) + camera.dir();
                            Vec3 pp = camera.ori() + dir * 150., loc = camera.ori() + (Vec3(rand01() * 1.05, rand01() - .5, 0.) * 2 * APERTURE);
                            std::vector<SPPMNode> tmp = sppmBacktrace(scene, Ray(pp, (loc - pp).normalized()), 0, y * WIDTH + x);
                            for (auto &nn : tmp)
                                if (nn.index > 0)
                                {
                                    nn.r = RADIUS;
                                    ball[threadNo].emplace_back(nn);
                                }
                        }
                }
            }
            cerr << "\rBuilding k-D tree: Gathering threads ...";
            std::vector<SPPMNode> total;
            for (size_t i = 0; i < nth; i++)
                total.insert(total.end(), ball[i].begin(), ball[i].end());
            // KDtree tree(total);
            tree.init(total);
            cerr << "Done!" << endl;
        }

        int per = SAMPLE / nth + 1;
#pragma omp parallel for num_threads(nth) schedule(dynamic)
        for (size_t t = 0; t < nth; t++)
        {
            int threadNo = omp_get_thread_num();
            for (size_t photon = 0; photon < per; photon++)
            {
                if (threadNo == 0 && photon % 1000 == 0)
                    cerr << "\rSPPM tracing " << 100. * photon / per;

                double rc = rand01() * 18, tht = rand01() * 2 * PI;
                Vec3 o(50 + rc * cos(tht), 81.6 - eps, 81.6 + rc * sin(tht));
                double r1 = 2 * PI * rand01(), r2 = rand01(), r2s = sqrt(r2);
                Vec3 w = Vec3(0, -1, 0), u = (Vec3(1, 0, 0).cross(w)).normalized(), v = w.cross(u);
                Vec3 d = (u * cos(r1) * r2s + v * sin(r1) * r2s + w * sqrt(1 - r2)).normalized();
                Ray light = Ray(o, d);
                Vec3 col = Vec3(1, 1, 1) + .4;
                tree.query(SPPMNode(o, col, d), imgBuff[threadNo]);
                sppmForward(scene, light, 0, col, imgBuff[threadNo], &tree);
            }
        }

        memset(imgNow, 0, sizeof imgNow);
        for (size_t i = 0; i < nth; i++)
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
            sprintf(sout, "%s%03d.ppm", FILENAME.c_str(), iter);
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
