#include "plotfft.h"
#include "SeriesDataRef.h"
#include "external/fftw/api/fftw3.h"

PlotFFT::PlotFFT()
{
}

PlotFFT::~PlotFFT()
{
}

void PlotFFT::FFTW(const std::vector<double>& key, const std::vector<double>& data, const int& scall, std::vector<FFTData> &dataFFT)
{
    fftw_complex *in, *out;
    fftw_plan p;
    int N= data.size();
    if (N & 1)
       N--;

    if(N < 8)
        return;

    double dt = ((key[N-1] - key[0]) / double(N - 1)) / scall; //sampling interval (s)
    double fs = 1/dt; //frequency sample

    in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);
    out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);
    for(int i=0; i < N; i++)
    {
        in[i][0] = data[i];
        in[i][1] = 0.0;
    }

    p = fftw_plan_dft_1d(N,in,out, FFTW_FORWARD, FFTW_ESTIMATE);
    fftw_execute(p);
    fftw_destroy_plan(p);

    dataFFT.resize(N / 2 + 1);
    //amplitude-frequency
    for(int i = 0; i< N / 2; i++)
    {
        FFTData data;
        data.key = i * fs / double(N); // Hz
        data.value = std::hypot(out[i][0], out[i][1]) / N; //amplitude
        dataFFT.push_back(std::move(data));
    }
    fftw_free(in);
    fftw_free(out);
}
