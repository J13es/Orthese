#include "EMGFiltersT.h"

// coefficients of transfer function of LPF
// coef[sampleFreqInd][order]
static float lpf_numerator_coefT[2][3] = {{0.3913, 0.7827, 0.3913},
                                          {0.1311, 0.2622, 0.1311}};
static float lpf_denominator_coefT[2][3] = {{1.0000, 0.3695, 0.1958},
                                            {1.0000, -0.7478, 0.2722}};
// coefficients of transfer function of HPF
static float hpf_numerator_coefT[2][3] = {{0.8371, -1.6742, 0.8371},
                                          {0.9150, -1.8299, 0.9150}};
static float hpf_denominator_coefT[2][3] = {{1.0000, -1.6475, 0.7009},
                                            {1.0000, -1.8227, 0.8372}};
// coefficients of transfer function of anti-hum filter
// coef[sampleFreqInd][order] for 50Hz
static float ahf_numerator_coef_50HzT[2][6] = {
    {0.9522, -1.5407, 0.9522, 0.8158, -0.8045, 0.0855},
    {0.5869, -1.1146, 0.5869, 1.0499, -2.0000, 1.0499}};
static float ahf_denominator_coef_50HzT[2][6] = {
    {1.0000, -1.5395, 0.9056, 1.0000 - 1.1187, 0.3129},
    {1.0000, -1.8844, 0.9893, 1.0000, -1.8991, 0.9892}};
static float ahf_output_gain_coef_50HzT[2] = {1.3422, 1.4399};
// coef[sampleFreqInd][order] for 60Hz
static float ahf_numerator_coef_60HzT[2][6] = {
    {0.9528, -1.3891, 0.9528, 0.8272, -0.7225, 0.0264},
    {0.5824, -1.0810, 0.5824, 1.0736, -2.0000, 1.0736}};
static float ahf_denominator_coef_60HzT[2][6] = {
    {1.0000, -1.3880, 0.9066, 1.0000, -0.9739, 0.2371},
    {1.0000, -1.8407, 0.9894, 1.0000, -1.8584, 0.9891}};
static float ahf_output_gain_coef_60HzT[2] = {1.3430, 1.4206};

enum FILTER_TYPET {
    FILTER_TYPE_LOWPASST = 0,
    FILTER_TYPE_HIGHPASST,
};

class FILTER_2ndT {
  private:
    // second-order filter
    float statesT[2];
    float numT[3];
    float denT[3];

  public:
    void init(FILTER_TYPET ftypeT, int sampleFreqT) {
        statesT[0] = 0;
        statesT[1] = 0;
        if (ftypeT == FILTER_TYPE_LOWPASST) {
            // 2th order butterworth lowpass filter
            // cutoff frequency 150Hz
            if (sampleFreqT == SAMPLE_FREQ_500HZ_T) {
                for (int i = 0; i < 3; i++) {
                    numT[i] = lpf_numerator_coefT[0][i];
                    denT[i] = lpf_denominator_coefT[0][i];
                }
            } else if (sampleFreqT == SAMPLE_FREQ_1000HZ_T) {
                for (int i = 0; i < 3; i++) {
                    numT[i] = lpf_numerator_coefT[1][i];
                    denT[i] = lpf_denominator_coefT[1][i];
                }
            }
        } else if (ftypeT == FILTER_TYPE_HIGHPASST) {
            // 2th order butterworth
            // cutoff frequency 20Hz
            if (sampleFreqT == SAMPLE_FREQ_500HZ_T) {
                for (int i = 0; i < 3; i++) {
                    numT[i] = hpf_numerator_coefT[0][i];
                    denT[i] = hpf_denominator_coefT[0][i];
                }
            } else if (sampleFreqT == SAMPLE_FREQ_1000HZ_T) {
                for (int i = 0; i < 3; i++) {
                    numT[i] = hpf_numerator_coefT[1][i];
                    denT[i] = hpf_denominator_coefT[1][i];
                }
            }
        }
    }

    float update(float inputT) {
        float tmpT = (inputT - denT[1] * statesT[0] - denT[2] * statesT[1]) / denT[0];
        float outputT = numT[0] * tmpT + numT[1] * statesT[0] + numT[2] * statesT[1];
        // save last states
        statesT[1] = statesT[0];
        statesT[0] = tmpT;
        return outputT;
    }
};

class FILTER_4thT {
  private:
    // fourth-order filter
    // cascade two 2nd-order filters
    float statesT[4];
    float numT[6];
    float denT[6];
    float gainT;

  public:
    void init(int sampleFreqT, int humFreqT) {  // probleme
        gainT = 0;
        for (int i = 0; i < 4; i++) {
            statesT[i] = 0;
        }
        if (humFreqT == NOTCH_FREQ_50HZ_T) {
            if (sampleFreqT == SAMPLE_FREQ_500HZ_T) {
                for (int i = 0; i < 6; i++) {
                    numT[i] = ahf_numerator_coef_50HzT[0][i];
                    denT[i] = ahf_denominator_coef_50HzT[0][i];
                }
                gainT = ahf_output_gain_coef_50HzT[0];
            } else if (sampleFreqT == SAMPLE_FREQ_1000HZ_T) {
                for (int i = 0; i < 6; i++) {
                    numT[i] = ahf_numerator_coef_50HzT[1][i];
                    denT[i] = ahf_denominator_coef_50HzT[1][i];
                }
                gainT = ahf_output_gain_coef_50HzT[1];
            }
        } else if (humFreqT == NOTCH_FREQ_60HZ_T) {
            if (sampleFreqT == SAMPLE_FREQ_500HZ_T) {
                for (int i = 0; i < 6; i++) {
                    numT[i] = ahf_numerator_coef_60HzT[0][i];
                    denT[i] = ahf_denominator_coef_60HzT[0][i];
                }
                gainT = ahf_output_gain_coef_60HzT[0];
            } else if (sampleFreqT == SAMPLE_FREQ_1000HZ_T) {
                for (int i = 0; i < 6; i++) {
                    numT[i] = ahf_numerator_coef_60HzT[1][i];
                    denT[i] = ahf_denominator_coef_60HzT[1][i];
                }
                gainT = ahf_output_gain_coef_60HzT[1];
            }
        }
    }

    float update(float inputT) {
        float outputT;
        float stageInT;
        float stageOutT;

        stageOutT  = numT[0] * inputT + statesT[0];
        statesT[0] = (numT[1] * inputT + statesT[1]) - denT[1] * stageOutT;
        statesT[1] = numT[2] * inputT - denT[2] * stageOutT;
        stageInT   = stageOutT;
        stageOutT  = numT[3] * stageOutT + statesT[2];
        statesT[2] = (numT[4] * stageInT + statesT[3]) - denT[4] * stageOutT;
        statesT[3] = numT[5] * stageInT - denT[5] * stageOutT;

        outputT = gainT * stageOutT;

        return outputT;
    }
};

FILTER_2ndT LPFT;
FILTER_2ndT HPFT;
FILTER_4thT AHFT;

void EMGFiltersT::init(SAMPLE_FREQUENCY_T sampleFreqT,
                      NOTCH_FREQUENCY_T  notchFreqT,
                      bool              enableNotchFilterT,
                      bool              enableLowpassFilterT,
                      bool              enableHighpassFilterT) {
    m_sampleFreqT   = sampleFreqT;
    m_notchFreqT    = notchFreqT;
    m_bypassEnabledT = true;
    if (((sampleFreqT == SAMPLE_FREQ_500HZ_T) || (sampleFreqT == SAMPLE_FREQ_1000HZ_T)) &&
        ((notchFreqT == NOTCH_FREQ_50HZ_T) || (notchFreqT == NOTCH_FREQ_60HZ_T))) {
            m_bypassEnabledT = false;
    }

    LPFT.init(FILTER_TYPE_LOWPASST, m_sampleFreqT);
    HPFT.init(FILTER_TYPE_HIGHPASST, m_sampleFreqT);
    AHFT.init(m_sampleFreqT, m_notchFreqT);

    m_notchFilterEnabledT    = enableNotchFilterT;
    m_lowpassFilterEnabledT  = enableLowpassFilterT;
    m_highpassFilterEnabledT = enableHighpassFilterT;
}

int EMGFiltersT::update(int inputValueT) {
    int outputT = 0;
    if (m_bypassEnabledT) {
        return outputT = inputValueT;
    }

    // first notch filter
    if (m_notchFilterEnabledT) {
        // outputT = NTF.update(inputValueT);
        outputT = AHFT.update(inputValueT);
    } else {
        // notch filter bypass
        outputT = inputValueT;
    }

    // second low pass filter
    if (m_lowpassFilterEnabledT) {
        outputT = LPFT.update(outputT);
    }

    // third high pass filter
    if (m_highpassFilterEnabledT) {
        outputT = HPFT.update(outputT);
    }

    return outputT;
}
