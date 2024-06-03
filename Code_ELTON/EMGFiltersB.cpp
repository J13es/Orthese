#include "EMGFiltersB.h"

// coefficients of transfer function of LPF
// coef[sampleFreqInd][order]
static float lpf_numerator_coefB[2][3] = {{0.3913, 0.7827, 0.3913},
                                          {0.1311, 0.2622, 0.1311}};
static float lpf_denominator_coefB[2][3] = {{1.0000, 0.3695, 0.1958},
                                            {1.0000, -0.7478, 0.2722}};
// coefficients of transfer function of HPF
static float hpf_numerator_coefB[2][3] = {{0.8371, -1.6742, 0.8371},
                                          {0.9150, -1.8299, 0.9150}};
static float hpf_denominator_coefB[2][3] = {{1.0000, -1.6475, 0.7009},
                                            {1.0000, -1.8227, 0.8372}};
// coefficients of transfer function of anti-hum filter
// coef[sampleFreqInd][order] for 50Hz
static float ahf_numerator_coef_50HzB[2][6] = {
    {0.9522, -1.5407, 0.9522, 0.8158, -0.8045, 0.0855},
    {0.5869, -1.1146, 0.5869, 1.0499, -2.0000, 1.0499}};
static float ahf_denominator_coef_50HzB[2][6] = {
    {1.0000, -1.5395, 0.9056, 1.0000 - 1.1187, 0.3129},
    {1.0000, -1.8844, 0.9893, 1.0000, -1.8991, 0.9892}};
static float ahf_output_gain_coef_50HzB[2] = {1.3422, 1.4399};
// coef[sampleFreqInd][order] for 60Hz
static float ahf_numerator_coef_60HzB[2][6] = {
    {0.9528, -1.3891, 0.9528, 0.8272, -0.7225, 0.0264},
    {0.5824, -1.0810, 0.5824, 1.0736, -2.0000, 1.0736}};
static float ahf_denominator_coef_60HzB[2][6] = {
    {1.0000, -1.3880, 0.9066, 1.0000, -0.9739, 0.2371},
    {1.0000, -1.8407, 0.9894, 1.0000, -1.8584, 0.9891}};
static float ahf_output_gain_coef_60HzB[2] = {1.3430, 1.4206};

enum FILTER_TYPEB {
    FILTER_TYPE_LOWPASSB = 0,
    FILTER_TYPE_HIGHPASSB,
};

class FILTER_2ndB {
  private:
    // second-order filter
    float statesB[2];
    float numB[3];
    float denB[3];

  public:
    void init(FILTER_TYPEB ftypeB, int sampleFreqB) {
        statesB[0] = 0;
        statesB[1] = 0;
        if (ftypeB == FILTER_TYPE_LOWPASSB) {
            // 2th order butterworth lowpass filter
            // cutoff frequency 150Hz
            if (sampleFreqB == SAMPLE_FREQ_500HZ_B) {
                for (int i = 0; i < 3; i++) {
                    numB[i] = lpf_numerator_coefB[0][i];
                    denB[i] = lpf_denominator_coefB[0][i];
                }
            } else if (sampleFreqB == SAMPLE_FREQ_1000HZ_B) {
                for (int i = 0; i < 3; i++) {
                    numB[i] = lpf_numerator_coefB[1][i];
                    denB[i] = lpf_denominator_coefB[1][i];
                }
            }
        } else if (ftypeB == FILTER_TYPE_HIGHPASSB) {
            // 2th order butterworth
            // cutoff frequency 20Hz
            if (sampleFreqB == SAMPLE_FREQ_500HZ_B) {
                for (int i = 0; i < 3; i++) {
                    numB[i] = hpf_numerator_coefB[0][i];
                    denB[i] = hpf_denominator_coefB[0][i];
                }
            } else if (sampleFreqB == SAMPLE_FREQ_1000HZ_B) {
                for (int i = 0; i < 3; i++) {
                    numB[i] = hpf_numerator_coefB[1][i];
                    denB[i] = hpf_denominator_coefB[1][i];
                }
            }
        }
    }

    float update(float inputB) {
        float tmpB = (inputB - denB[1] * statesB[0] - denB[2] * statesB[1]) / denB[0];
        float outputB = numB[0] * tmpB + numB[1] * statesB[0] + numB[2] * statesB[1];
        // save last states
        statesB[1] = statesB[0];
        statesB[0] = tmpB;
        return outputB;
    }
};

class FILTER_4thB {
  private:
    // fourth-order filter
    // cascade two 2nd-order filters
    float statesB[4];
    float numB[6];
    float denB[6];
    float gainB;

  public:
    void init(int sampleFreqB, int humFreqB) {  // probleme
        gainB = 0;
        for (int i = 0; i < 4; i++) {
            statesB[i] = 0;
        }
        if (humFreqB == NOTCH_FREQ_50HZ_B) {
            if (sampleFreqB == SAMPLE_FREQ_500HZ_B) {
                for (int i = 0; i < 6; i++) {
                    numB[i] = ahf_numerator_coef_50HzB[0][i];
                    denB[i] = ahf_denominator_coef_50HzB[0][i];
                }
                gainB = ahf_output_gain_coef_50HzB[0];
            } else if (sampleFreqB == SAMPLE_FREQ_1000HZ_B) {
                for (int i = 0; i < 6; i++) {
                    numB[i] = ahf_numerator_coef_50HzB[1][i];
                    denB[i] = ahf_denominator_coef_50HzB[1][i];
                }
                gainB = ahf_output_gain_coef_50HzB[1];
            }
        } else if (humFreqB == NOTCH_FREQ_60HZ_B) {
            if (sampleFreqB == SAMPLE_FREQ_500HZ_B) {
                for (int i = 0; i < 6; i++) {
                    numB[i] = ahf_numerator_coef_60HzB[0][i];
                    denB[i] = ahf_denominator_coef_60HzB[0][i];
                }
                gainB = ahf_output_gain_coef_60HzB[0];
            } else if (sampleFreqB == SAMPLE_FREQ_1000HZ_B) {
                for (int i = 0; i < 6; i++) {
                    numB[i] = ahf_numerator_coef_60HzB[1][i];
                    denB[i] = ahf_denominator_coef_60HzB[1][i];
                }
                gainB = ahf_output_gain_coef_60HzB[1];
            }
        }
    }

    float update(float inputB) {
        float outputB;
        float stageInB;
        float stageOutB;

        stageOutB  = numB[0] * inputB + statesB[0];
        statesB[0] = (numB[1] * inputB + statesB[1]) - denB[1] * stageOutB;
        statesB[1] = numB[2] * inputB - denB[2] * stageOutB;
        stageInB   = stageOutB;
        stageOutB  = numB[3] * stageOutB + statesB[2];
        statesB[2] = (numB[4] * stageInB + statesB[3]) - denB[4] * stageOutB;
        statesB[3] = numB[5] * stageInB - denB[5] * stageOutB;

        outputB = gainB * stageOutB;

        return outputB;
    }
};

FILTER_2ndB LPFB;
FILTER_2ndB HPFB;
FILTER_4thB AHFB;

void EMGFiltersB::init(SAMPLE_FREQUENCY_B sampleFreqB,
                      NOTCH_FREQUENCY_B  notchFreqB,
                      bool              enableNotchFilterB,
                      bool              enableLowpassFilterB,
                      bool              enableHighpassFilterB) {
    m_sampleFreqB   = sampleFreqB;
    m_notchFreqB    = notchFreqB;
    m_bypassEnabledB = true;
    if (((sampleFreqB == SAMPLE_FREQ_500HZ_B) || (sampleFreqB == SAMPLE_FREQ_1000HZ_B)) &&
        ((notchFreqB == NOTCH_FREQ_50HZ_B) || (notchFreqB == NOTCH_FREQ_60HZ_B))) {
            m_bypassEnabledB = false;
    }

    LPFB.init(FILTER_TYPE_LOWPASSB, m_sampleFreqB);
    HPFB.init(FILTER_TYPE_HIGHPASSB, m_sampleFreqB);
    AHFB.init(m_sampleFreqB, m_notchFreqB);

    m_notchFilterEnabledB    = enableNotchFilterB;
    m_lowpassFilterEnabledB  = enableLowpassFilterB;
    m_highpassFilterEnabledB = enableHighpassFilterB;
}

int EMGFiltersB::update(int inputValueB) {
    int outputB = 0;
    if (m_bypassEnabledB) {
        return outputB = inputValueB;
    }

    // first notch filter
    if (m_notchFilterEnabledB) {
        // outputB = NTF.update(inputValueB);
        outputB = AHFB.update(inputValueB);
    } else {
        // notch filter bypass
        outputB = inputValueB;
    }

    // second low pass filter
    if (m_lowpassFilterEnabledB) {
        outputB = LPFB.update(outputB);
    }

    // third high pass filter
    if (m_highpassFilterEnabledB) {
        outputB = HPFB.update(outputB);
    }

    return outputB;
}
