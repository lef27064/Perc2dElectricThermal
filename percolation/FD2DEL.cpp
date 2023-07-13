
#include "FD2DEL.h"
#include <cstring>
#include <cmath>
void FD2DEL::current(double* icurrx, double* icurry, double* iu, double* igx, double* igy)
{

    //initialize the volume averaged currents
    *icurrx = 0.0;
    *icurry = 0.0;
    double cur1 = 0;
    double cur2 = 0;
    int m = 0;
    //Only loop over real sites and bonds in order to get true total current
    for (int j = 2; j <= ny2 - 1; j++)
    {
        for (int i = 2; i <= nx2 - 1; i++)
        {
            m = nx2 * (j - 1) + i - 1;
            //cur1, cur2 are the currents in one pixel
            cur1 = 0.5 * ((iu[m - 1] - iu[m]) * igx[m - 1] + (iu[m] - iu[m + 1]) * igx[m]);
            cur2 = 0.5 * ((iu[m - nx2] - iu[m]) * igy[m - nx2] + (iu[m] - iu[m + nx2]) * igy[m]);


            //sum pixel currents into volume averages
            *icurrx = *icurrx + cur1;
            *icurry = *icurry + cur2;
        }
    }

    *icurrx = *icurrx / double(nx * ny);
    *icurry = *icurry / double(nx * ny);

    return;

}

void FD2DEL::ppixel(int* ipix, double* ia, int nphase, int ntot)
{




    for (int j = 2; j <= ny + 1; j++)
        for (int i = 2; i <= nx + 1; i++)
        {
            int m=(j - 1)* (nx + 2) + i - 1;

            ia[ipix[m]] = ia[ipix[m]] + 1.0;
        }


    for (int i = 0; i < nphase; i++)
        ia[i] = ia[i] / double(nx * ny);

    int j1,i1,m,m1;


    //  now map periodic boundaries of pix(see Section 3.3, Figure 3 in manual)
   //  now map periodic boundaries of pix(see Section 3.3, Figure 3 in manual)
    for (int j = 0; j < ny2; j++)
        for (int i = 0; i < nx2; i++)
        {
            if ((i == 0) || (i == (nx2 - 1)) || (j == 0) || (j == (ny2 - 1)))

            {
                j1 = j;
                if (j == 0)
                    j1 = j + ny;

                if (j == ny2 - 1)
                    j1 = j - ny;
                i1 = i;
                if (i == 0)
                    i1 = i + nx;
                if (i == nx2 - 1)
                    i1 = i - nx;

                m = (j)*nx2 + i;
                m1 = (j1)*nx2 + i1;

                ipix[m] = ipix[m1];
            }


        }

    // Check for wrong phase labels--less than 1 or greater than nphase
    for (int im = 0; im < ns2 - 1; im++)
    {
        if (ipix[im] < 0)
            std::cout << "Phase label in pix < 0--error at " << im << "\n";


        if (ipix[im] > nphase - 1)
            std::cout << "Phase label in pix > nphase--error at " << im << "\n";
    }

}
//Subroutine that determines the correct bond conductances that are used
//to compute multiplication by the matrix A

void FD2DEL::bond(int* ipix, double* igx, double* igy, Matrix2D<double>* isigma, Matrix3D<double>* ibe, int nphase, int ntot)
{
    // auxiliary variables involving the system size


    int j1;
    int i1;
    int m1;

    int i, j, m;
    // Set values of conductor for phase(i)--phase(j) interface,
    // store in array ibe(i,j,m). If either phase i or j has zero conductivity,
    // then ibe(i,j,m)=0.
    for (m = 0; m < 2; ++m) {
        for (i = 0; i < nphase; ++i) {
            for (j = 0; j < nphase; ++j) {
                if (isigma->at(i, m) == 0.0)
                    ibe->set(i, j, m, 0.0);
                else
                    if (isigma->at(j, m) == 0.0)
                        ibe->set(i, j, m, 0.0);
                    else
                        ibe->set(i, j, m, 1. / ((0.5 / isigma->at(i, m)) + (0.5 / isigma->at(j, m))));
                //std::cout << "ibe[" << i << "," << j << "," << m << "]=" << ibe->at(i, j, m) << "\n";

            }
        }
    }


    // Trim off x face so that no current can flow past periodic
    // boundaries.  This step is not really necessary, as the voltages on the
    // periodic boundaries will be matched to the corresponding real voltages
    // in each conjugate gradient step.
    for (j = 1; j < ny2; ++j)
    {
        igx[nx2 * (j - 1) + nx2 - 1] = 0.0;
    }

    // bulk---igy
    for (i = 1; i <= nx2; ++i)
    {
        for (j = 1; j <= ny1; ++j)
        {
            m = (j - 1) * nx2 + i;
            j1 = j + 1;
            i1 = i;
            m1 = (j1 - 1) * nx2 + i1;
            igy[m - 1] = ibe->at(ipix[m - 1], ipix[m1 - 1], 1);
            //         std::cout << m-1 << "   " << m1 - 1 << "   "  << ipix[m-1] << "   " << ipix[m1 - 1] << "   " <<igy[m - 1] << "\n";
        }
    }
    // bulk--igx
    for (i = 1; i <= nx1; ++i) {
        for (j = 1; j <= ny2; ++j) {
            m = (j - 1) * nx2 + i;
            i1 = i + 1;
            j1 = j;
            m1 = (j1 - 1) * nx2 + i1;
            igx[m - 1] = ibe->at(ipix[m - 1], ipix[m1 - 1], 0);
            //          std::cout << m - 1 << "   " << m - 1 << "   " << ipix[m - 1] << "   " << pix[m - 1] << "   " << igx[m - 1] << "\n";

        }
    }
    return;
}

void  FD2DEL::prod(double* igx, double* igy, double* xw, double* yw)
{
    //using namespace std;
    //real igx(ns2), gy(ns2), xw(ns2), yw(ns2);

    // xw is the input vector, yw = (A)(xw) is the output vector


    int i, j;
    // Perform basic matrix multiplication, results in incorrect information at
    // periodic boundaries.
    std::fill_n(yw, ns2, 0);


    for (i = nx2 + 1; i <= ns2 - nx2; ++i)
    {
        yw[i] = -xw[i] * (igx[i - 1] + igx[i] + igy[i] + igy[i - nx2]);
        //  std::cout << "stage 1 yw[i]=" << yw[i] << "\n";

        yw[i] = yw[i] + igx[i - 1] * xw[i - 1] + igx[i] * xw[i + 1] + igy[i] * xw[i + nx2] + igy[i - nx2] * xw[i - nx2];

        //   std::cout << "yw[i]=" << yw[i] << "\n";
    }
    // Correct terms at periodic boundaries (Section 3.3 in manual)

    // x faces
    for (j = 1; j <= ny2; ++j)
    {
        yw[nx2 * (j - 1) + nx2 - 1] = yw[nx2 * (j - 1) + 1];
        yw[nx2 * (j - 1)] = yw[nx2 * (j - 1) + nx1 - 1];
    }

    // y faces
    for (i = 1; i <= nx2; ++i)
    {
        yw[i - 1] = yw[ny * nx2 + i - 1];
        yw[ny1 * nx2 + i - 1] = yw[nx2 + i - 1];
    }
    return;
}



void FD2DEL::dembx(double* igx, double* igy, double* iu, int* ic, double* gb, double* h, double* Ah, int* list, int nlist, double gtest)
{
    double gg, hAh, lambda, gglast, gamma, currx, curry;
    // Note:  voltage gradients are maintained because in the conjugate gradient
    // relaxation algorithm, the voltage vector is only modified by adding a
    // periodic vector to it.

    // First stage, compute initial value of gradient (gb), initialize h, the
    // conjugate gradient direction, and compute norm squared of gradient vector.

    prod(igx, igy, iu, gb);
    for (int i = 0; i < ns2; ++i)
        h[i] = gb[i];


    // Variable gg is the norm squared of the gradient vector
    gg = 0.0;
    for (int k = 0; k < nlist; ++k)
    {
        int m = list[k];
        gg = (gb[m - 1] * gb[m - 1]) + gg;


    }
    hAh = 0;
    // Second stage, initialize Ah variable, compute parameter lamdba,
    // make first change in voltage array, update gradient (gb) vector
    int icc = 0;
    if (gg > gtest)
    {
        prod(igx, igy, h, Ah);

        for (int k = 1; k <= nlist; ++k)
        {
            int m = list[k - 1];
            hAh = hAh + (h[m - 1] * Ah[m - 1]);

        }
        lambda = gg / hAh;
        for (int i = 0; i < ns2; ++i)
        {
            iu[i] = iu[i] - lambda * h[i];
            gb[i] = gb[i] - lambda * Ah[i];
        }

        // third stage:  iterate conjugate gradient solution process until
        // real(gg) < gtest criterion is satisfied.
        // (USER) The parameter ncgsteps is the total number of conjugate gradient steps
        // to go through.  Only in very unusual problems, like when the conductivity
        // of one phase is much higher than all the rest, will this many steps be
        // used.

        gg = 1e25;
        for (icc = 0; ((icc < ncgsteps) && (gg > gtest)); ++icc)
        {
            gglast = gg;
            gg = 0.0;
            for (int k = 0; k < nlist; ++k)
            {
                int m = list[k];
                gg = (gb[m - 1] * gb[m - 1]) + gg;
            }


            gamma = gg / gglast;
            // update conjugate gradient direction
            for (int i = 0; i < ns2; ++i)
            {
                h[i] = gb[i] + gamma * h[i];
            }

            prod(igx, igy, h, Ah);
            hAh = 0.0;
            for (int k = 0; k < nlist; ++k)
            {
                int m = list[k];
                hAh = hAh + h[m - 1] * Ah[m - 1];
            }

            lambda = gg / hAh;
            // update voltage, gradient vectors
            for (int i = 0; i < ns2; ++i)
            {
                iu[i] = iu[i] - (lambda * h[i]);
                gb[i] = gb[i] - (lambda * Ah[i]);
            }

            // (USER) This piece of code forces dembx to write out the total current and
            // the norm of the gradient squared, every ncheck conjugate gradient steps,
            // in order to see how the relaxation is proceeding. If the currents become
            // unchanging before the relaxation is done, then gtest was picked to be
            // smaller than was necessary.

            int ncheck = 5000;

            if ((icc % ncheck) == 0)
            {
                std::cout << icc;
                std::cout << " gg = " << gg << "\n";
                // call current subroutine
                current(&currx, &curry, iu, igx, igy);
                std::cout << " currx = " << currx << "\n";
                std::cout << " curry = " << curry << "\n";
                std::cout << "number of conjugate gradient cycles needed = " << icc << "\n";
            }

        }
        std::cout << " iteration failed to converge after" << ncgsteps << " steps";
    }

    *ic = icc;
    return;
}

void FD2DEL::initValues(int x, int y)
{
    width = x; heigth = y;
    nx = x;  ny = y;  nx1 = x + 1;  ny1 = y + 1;  nx2 = x + 2;  ny2 = y + 2;
    ns2 = nx2 * ny2;
}

void FD2DEL::intitArrays() {
    //allocate memory

    gx = new double[ns2];
    std::memset(gx, 0, ns2);

    gy = new double[ns2];
    memset(gy, 0, ns2);

    u = new double[ns2];
    memset(u, 0, ns2);

    gb = new double[ns2];
    memset(gb, 0, ns2);

    h = new double[ns2];
    memset(h, 0, ns2);

    ah = new double[ns2];
    memset(ah, 0, ns2);

    a = new double[ns2];
    memset(a, 0, ns2);

    pix = new int[ns2];;
    memset(pix, 0, ns2);

    list = new int[ns2];
    memset(list, 0, ns2);
}

FD2DEL::FD2DEL(int x, int y, int icomponents, int i_maxComponents) :width(x), heigth(y),
nx(x), ny(y), nx1(x + 1), ny1(y + 1), nx2(x + 2), ny2(y + 2), components(icomponents), _maxComponents(i_maxComponents)
{
    int size = (nx2) * (ny2);
    ns2 = size;
    intitArrays();

    //sigma = new Matrix2D<double>(_maxComponents, 2);
    //be = new Matrix3D<double>(_maxComponents, _maxComponents, 2);


}


FD2DEL::FD2DEL(int icomponents, int i_maxComponents) : components(icomponents), _maxComponents(i_maxComponents)
{

    sigma = new Matrix2D<double>(_maxComponents, 2);
    be = new Matrix3D<double>(_maxComponents, _maxComponents, 2);


}


FD2DEL::~FD2DEL(void)
{
    delete[] gx;
    delete[] gy;
    delete[] u;
    delete[] gb;
    delete[] h;
    delete[] ah;
    delete[] a;
    delete[] pix;
    delete[] list;
    sigma->~Matrix2D();
    be->~Matrix3D();


}

// void FD2DEL::run(char* inputFileName, char* outputFileName, int nx, int ny)
// {}
//

void FD2DEL::readFromFile(char* inputFileName)
{
    std::string line;
    std::ifstream inputFile(inputFileName);
    if (inputFile.is_open())
    {
        for (int i = 2; i <= nx + 1; i++)
            for (int j = 1; j <= ny + 1; j++)
            {
                getline(inputFile, line);
                int m = (j - 1) * (nx + 2) + i;
                pix[m - 1] = stoi(line) - 1;
            }

    }
}
/*
long GetFileSize(std::string filename)
{
    struct stat stat_buf;
    int rc = stat(filename.c_str(), &stat_buf);
    return rc == 0 ? stat_buf.st_size : -1;
}
*/


std::vector<std::string> FD2DEL::split(std::string strToSplit, char delimeter)
{
    std::stringstream ss(strToSplit);
    std::string item;
    std::vector<std::string> splittedStrings;
    while (std::getline(ss, item, delimeter))
    {
        splittedStrings.push_back(item);
    }
    return splittedStrings;
}


long FD2DEL::readFromImageFile(std::string filename, std::list<int>* pixels)
{
    std::string line;
    std::ifstream inputFile(filename);
    std::vector<std::string> strComponent;
    long long count = 0;
    while (getline(inputFile, line))
    {
        strComponent = split(line, ' ');
        for (int i = 0; i < strComponent.size(); i++)
        {
            pixels->push_back(std::stoi(strComponent[i]));
        }
        count = count + strComponent.size();
    }
    inputFile.close();
    return count;
}

long FD2DEL::readFromImageFile(std::string filename, std::list<int>* pixels, int* x, int* y)
{
    std::string line;
    std::ifstream inputFile(filename);
    std::vector<std::string> strComponent;
    long count = 0;
    *x = 0;
    *y = 0;
    while (getline(inputFile, line))
    {
        strComponent = split(line, ' ');
        *x = strComponent.size();
        for (int i = 0; i < strComponent.size(); i++)
        {
            pixels->push_back(std::stoi(strComponent[i]));
        }
        count = count + strComponent.size();
        *y = *y + 1;
    }
    inputFile.close();
    return count;
}


long FD2DEL::GetFileSize(std::string filename, int* x, int* y)
{
    std::string line;
    std::ifstream inputFile(filename);
    std::vector<std::string> strComponent;
    long count = 0;
    *x = 0;
    *y = 0;
    while (getline(inputFile, line))
    {
        strComponent = split(line, ' ');
        *x = strComponent.size();

        count = count + *x;
        *y = *y + 1;
    }
    inputFile.close();
    return count;
}

void FD2DEL::readFromImageFileSequantially(char* inputFileName)
{

    std::string line;
    std::ifstream inputFile(inputFileName);

    if (inputFile.is_open())
    {
        for (int i = 2; i <= nx + 1; i++)
            for (int j = 1; j < ny + 1; j++)
            {
                getline(inputFile, line);
                int m = (j - 1) * (nx + 2) + i;
                pix[m - 1] = stoi(line) - 1;
            }

    }
}


void FD2DEL::readFromArray(unsigned char* ingadients)
{


    for (int i = 2; i <= nx + 1; i++)
        for (int j = 1; j <= ny + 1; j++)
        {
            int m = (j - 1) * nx2 + i;
            int m1 = ((j - 1) * nx) + i - 2;
            pix[m - 1] = ingadients[m1];
        }


}

void FD2DEL::readFromArray(unsigned char* ingadients, double* iMaterialsElecricConductivity)
{



    for (int j = 2; j <= ny + 1; j++)
        for (int i = 2; i <= nx + 1; i++)
        {
            int m = (j - 1) * nx2 + i-1;
            int m1 = ((j - 2) * nx) + i - 2;
            pix[m] = ingadients[m1];
        }


    for (int i = 0; i < components; i++)
    {
        sigma->set(i, 0, iMaterialsElecricConductivity[i]);
        sigma->set(i, 1, iMaterialsElecricConductivity[i]);
    }

}



void FD2DEL::readFromImageFile(char* inputFileName, int* x, int* y)
{
    std::list<int> data;
    long fileSize = readFromImageFile(inputFileName, &data, x, y);
    if ((*x == 1) || (*y == 1))
    {
        *x = int(std::sqrt(fileSize));
        *y = *x;


        initValues(*x, *y);
        intitArrays();
        // readFromImageFileSequantially(inputFileName);
    }
    else
    {
        initValues(*x, *y);
        intitArrays();

    }
    //int count = 0;
    for (int j = 2; j <= ny + 1; j++)
        for (int i = 2; i <= nx + 1; i++)
        {
            //    count++;
            //    std::cout << count << "\n";

            int m = (j - 1) * nx2 + i;
            pix[m - 1] = data.front() - 1;

            data.pop_front();
            std::cout << m - 1 << "," << pix[m - 1] << "\n";
        }
}


void FD2DEL::readFromFile(char* inputFileName, int* x, int* y)
{

    long fileSize = GetFileSize(inputFileName, x, y);
    if ((*x == 1) || (*y == 1))
    {
        *x = int(sqrt(fileSize));
        *y = *x;

        initValues(*x, *y);
        intitArrays();
        // readFromImageFileSequantially(inputFileName);
    }
    else
    {
        initValues(*x, *y);
        intitArrays();

    }


    //FD2DEL(*x, *y, components, _maxComponents);
    std::string line;
    std::ifstream inputFile(inputFileName);

    if (inputFile.is_open())
    {
        for (int j = 2; j <= ny2 - 1; j++)
            for (int i = 2; i <= ny2 - 1; i++)
            {
                getline(inputFile, line);
                int m = (j - 1) * nx2 + i;
                pix[m - 1] = stoi(line) - 1;
                std::cout << m << "," << pix[m - 1] << "\n";
            }

    }
    inputFile.close();
}

void FD2DEL::run(char* inputFileName, char* outputFileName, int inphase)
{


    std::cout << "image is " << nx << " x " << ny << " no of real sites =" << nx * ny << "\n";




    // (USER) nphase is the number of phases being considered in the problem.
    // The values of pix(m) will run from 1 to nphase.  ntot is the total
    // number of phases possible in the program, and is the dimension of
    // sigma, a, and be.

    int nphase = inphase;
    int ntot = 100;
    // Make list of real (interior) sites, used in subroutine dembx.  The 1-d
    // labelling scheme goes over all ns2 sites, so a list of the real sites
    // is needed.
    int nlist = 0;



    for (int i = 2; i <= nx1; ++i)
    {
        for (int j = 2; j <= ny1; ++j) {
            int m = i + (j - 1) * nx2;

            list[nlist] = m - 1;
            nlist++;
        }

    }

    gtest = 1.0e-16 * this->ns2;
    // Compute average current in each pixel.
    // (USER) npoints is the number of microstructures to use.

      ppixel(pix, a, nphase, ntot);
        // output phase volume fractions

      for (int i = 0; i < nphase; ++i)
        {
            std::cout << "phase fraction of " << i << " = " << a[i] << "\n";
        }

        // (USER) Set components of applied field, E = (ex,ey)
        double ex = 0.0;
        double ey = 1.0;

        std::cout << "Applied field components : ex =" << ex << " ey = " << ey << "\n";

        // Initialize the voltage distribution by putting on uniform field.
        for (int j = 1; j <= ny2; ++j)
        {
            for (int i = 0; i < nx2; ++i)
            {
                int m = (nx2 * (j - 1)) + i;
                u[m] = -ex * i - ey * j;

            }
        }

        // (USER) input value of real conductivity tensor for each phase
        // (diagonal only). 1,2 = x,y,  respectively.



        // bond() sets up conductor network in gx,gy 1-d arrays
        bond(pix, gx, gy, sigma, be, nphase, ntot);

        // Subroutine dembx accepts gx,gy and solves for the voltage fieldF
        // that minimizes the dissipated energy.
        int ic = 0;
        dembx(gx, gy, u, &ic, gb, h, ah, list, nlist, gtest);

        // find final current after voltage solution is done
        current(&currx, &curry, u, gx, gy);

        std::cout << "\naverage current in x direction = " << currx << "\n";
        std::cout << "average current in y direction = " << curry << "\n";
        std::cout << "number of conjugate gradient cycles needed = " << ic << "\n";





}
