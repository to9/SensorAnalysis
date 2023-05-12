#ifndef PLOTFFT_H
#define PLOTFFT_H

#include <QObject>

struct FFTData
{
    double key;
    double value;
};

class PlotFFT : public QObject
{
    Q_OBJECT
public:
    PlotFFT();
    virtual ~PlotFFT();

    void FFTW(const std::vector<double>& key, const std::vector<double>& data, const int &scall, std::vector<FFTData> &dataFFT);

};

#endif // PLOTFFT_H
