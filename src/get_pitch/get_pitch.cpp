/// @file

#include <iostream>
#include <fstream>
#include <string.h>
#include <errno.h>

#include "wavfile_mono.h"
#include "pitch_analyzer.h"

#include "docopt.h"

#define FRAME_LEN 0.030   /* 30 ms. */
#define FRAME_SHIFT 0.015 /* 15 ms. */

using namespace std;
using namespace upc;

static const char USAGE[] = R"(
get_pitch - Pitch Detector 

Usage:
    get_pitch [options] <input-wav> <output-txt>
    get_pitch (-h | --help)
    get_pitch --version

Options:
    -h, --help  Show this screen
    --version   Show the version of the project

Arguments:
    input-wav   Wave file with the audio signal
    output-txt  Output file: ASCII file with the result of the detection:
                    - One line per frame with the estimated f0
                    - If considered unvoiced, f0 must be set to f0 = 0
)";

int main(int argc, const char *argv[])
{
  /// \TODO
  ///  Modify the program syntax and the call to **docopt()** in order to
  ///  add options and arguments to the program.
  std::map<std::string, docopt::value> args = docopt::docopt(USAGE,
                                                             {argv + 1, argv + argc}, // array of arguments, without the program name
                                                             true,                    // show help if requested
                                                             "2.0");                  // version string

  std::string input_wav = args["<input-wav>"].asString();
  std::string output_txt = args["<output-txt>"].asString();

  // Read input sound file
  unsigned int rate;
  vector<float> x;
  if (readwav_mono(input_wav, rate, x) != 0)
  {
    cerr << "Error reading input file " << input_wav << " (" << strerror(errno) << ")\n";
    return -2;
  }

  int n_len = rate * FRAME_LEN;
  int n_shift = rate * FRAME_SHIFT;

  // Define analyzer
  PitchAnalyzer analyzer(n_len, rate, PitchAnalyzer::HAMMING, 50, 500);

  /// \TODO
  /// Preprocess the input signal in order to ease pitch estimation. For instance,
  /// central-clipping or low pass filtering may be used.

  //  Cálculo del Threshold CL para el center-clipping
  float firstMax = 0.0;
  float lastMax = 0.0;
  float CL = 0.0;
  float potencia = 0.0;

  /*for (unsigned int i = 0; i < x.size(); i++)
  {
    if (i < (unsigned int)(x.size() / 3))
    {
      if (abs(x[i]) > firstMax)
      {
        firstMax = abs(x[i]);
      }
    }
    if (i > (unsigned int)(2 * x.size() / 3))
    {
      if (abs(x[i])> lastMax)
      {
        lastMax = abs(x[i]);
      }
    }
  }

  if (firstMax > lastMax)
  {
    CL = 0.7 * lastMax;
  }
  else
  {
    CL = 0.7 * firstMax;
  }*/

  for (unsigned int i = 0; i < x.size(); i++){
      potencia += abs(x[i])*abs(x[i]);
  }
  potencia = (1.0F/x.size()) * potencia;
  CL = 0.3*potencia;
  
  //  Aplicamos el Center Clipping
  for (unsigned int i = 0; i < x.size(); i++)
  {
    if (x[i] >= CL)
    {
      x[i] -= CL;
    }
    else if (abs(x[i]) < CL)
    {
      x[i] = 0;
    }
    else if (x[i] <= -CL)
    {
      x[i] += CL;
    }
  }

  // Iterate for each frame and save values in f0 vector
  vector<float>::iterator iX;
  vector<float> f0;
  for (iX = x.begin(); iX + n_len < x.end(); iX = iX + n_shift)
  {
    float f = analyzer(iX, iX + n_len);
    f0.push_back(f);
  }

  /// \TODO
  /// Postprocess the estimation in order to supress errors. For instance, a median filter
  /// or time-warping may be used.

  // Write f0 contour into the output file
  ofstream os(output_txt);
  if (!os.good())
  {
    cerr << "Error reading output file " << output_txt << " (" << strerror(errno) << ")\n";
    return -3;
  }

  os << 0 << '\n'; //pitch at t=0
  for (iX = f0.begin(); iX != f0.end(); ++iX)
    os << *iX << '\n';
  os << 0 << '\n'; //pitch at t=Dur

  return 0;
}
