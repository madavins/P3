/// @file

#include <iostream>
#include <math.h>
#include <iomanip>
#include "pitch_analyzer.h"

using namespace std;

/// Name space of UPC
namespace upc
{
void PitchAnalyzer::autocorrelation(const vector<float> &x, vector<float> &r) const
{

  for (unsigned int l = 0; l < r.size(); ++l)
  {
    /// \DONE Compute the autocorrelation r[l]
    for (unsigned int n = 0; n < (x.size() - l); n++)
    {
      r[l] = r[l] + x[n] * x[n + l];
    }
    r[l] = (1.0F / x.size()) * r[l];
  }

  if (r[0] == 0.0F) //to avoid log() and divide zero
    r[0] = 1e-10;
}

void PitchAnalyzer::set_window(Window win_type)
{
  if (frameLen == 0)
    return;

  window.resize(frameLen);

  //HAMMING CONSTANTS
  const float a0 = 0.53836;
  const float a1 = 0.46164;

  switch (win_type)
  {
  case HAMMING:
    /// \DONE Implement the Hamming window
    for (unsigned int n = 0; n < frameLen; n++)
    {
      window[n] = a0 - a1 * cos((2 * M_PI * n) / (frameLen - 1));
    }
    break;
  case RECT:
  default:
    window.assign(frameLen, 1);
  }
}

void PitchAnalyzer::set_f0_range(float min_F0, float max_F0)
{
  npitch_min = (unsigned int)samplingFreq / max_F0;
  if (npitch_min < 2)
    npitch_min = 2; // samplingFreq/2

  npitch_max = 1 + (unsigned int)samplingFreq / min_F0;

  //frameLen should include at least 2*T0
  if (npitch_max > frameLen / 2)
    npitch_max = frameLen / 2;
}

bool PitchAnalyzer::unvoiced(float pot, float r1norm, float rmaxnorm) const
{
  /// \DONE Implement a rule to decide whether the sound is voiced or not.
  /// * You can use the standard features (pot, r1norm, rmaxnorm),
  ///   or compute and use other ones.

  //const float r1 = 0.84;
  //const float rmax = 0.285;
  //const float pot = -50.5;
  if (pot < -50.5 || r1norm < 0.84 || rmaxnorm < 0.285)
  {
    return true;
  }
  else
  {
    return false;
  }
}

float PitchAnalyzer::compute_pitch(vector<float> &x) const
{
  if (x.size() != frameLen)
    return -1.0F;

  //Window input frame
  for (unsigned int i = 0; i < x.size(); ++i)
    x[i] *= window[i];

  //Declaramos vector r de npitch_max posiciones
  vector<float> r(npitch_max);

  //Compute correlation
  autocorrelation(x, r);

  vector<float>::const_iterator iR = r.begin(), iRMax = iR;
  vector<float>::const_iterator iRanterior = r.begin();
  vector<float>::const_iterator iRposterior = r.begin();
  /// \DONE
  /// Find the lag of the maximum value of the autocorrelation away from the origin.<br>
  /// Choices to set the minimum value of the lag are:
  ///    - The first negative value of the autocorrelation.
  ///    - The lag corresponding to the maximum value of the pitch.
  ///	   .
  /// In either case, the lag should not exceed that of the minimum value of the pitch.

  /*bool zerocros = false;
  while (iR < r.begin() + npitch_min)
  {
    if (*iR < 0.0F)
    {
      zerocros = true;
    }
    ++iR;
  }
  if (!zerocros)
  {
    while (*iR > 0.0F)
    {
      ++iR;
    }
  }*/

  /*if (iR < r.begin() + npitch_min)
  {
    iR = r.begin()+ npitch_min;
  }*/

  //iRMax = iR;
  
  iRposterior = iR + 1;

  while(*iR > *iRposterior || iR < r.begin()+npitch_min || *iR > 0.0F){
    ++iR;
    ++iRposterior;
  }

  iRMax = iR;
  while (iR < r.begin() + npitch_max)
  {
    if (*iR > *iRMax)
    {
      iRanterior = iR - 1;
      iRposterior = iR + 1;
      if (*iR > *iRanterior && *iR > *iRposterior)
        iRMax = iR;
    }
    ++iR;
  }

  unsigned int lag = iRMax - r.begin();

  float pot = 10 * log10(r[0]);

  //You can print these (and other) features, look at them using wavesurfer
  //Based on that, implement a rule for unvoiced
  //change to #if 1 and compile
#if 1
  //if (r[0] > 0.0F)
  cout << fixed << setprecision(3) << pot << '\t' << '\t' << r[1]/r[0] << '\t' << '\t' << r[lag]/r[0]  << endl;
#endif

  if (unvoiced(pot, r[1] / r[0], r[lag] / r[0]))
    return 0;
  else
    return (float)samplingFreq / (float)lag;
}
} // namespace upc
