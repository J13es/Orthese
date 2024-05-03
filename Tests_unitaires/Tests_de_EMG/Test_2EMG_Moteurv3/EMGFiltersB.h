#ifndef _EMGFILTERS_B_H
#define _EMGFILTERS_B_H

enum NOTCH_FREQUENCY_B { NOTCH_FREQ_50HZ_B = 50, NOTCH_FREQ_60HZ_B = 60 };

enum SAMPLE_FREQUENCY_B { SAMPLE_FREQ_500HZ_B = 500, SAMPLE_FREQ_1000HZ_B = 1000 };

// Déclaration de la classe EMGFilters
class EMGFiltersB {
  public:
    // Méthode pour initialiser les filtres EMG
    void init(SAMPLE_FREQUENCY_B sampleFreqB,
              NOTCH_FREQUENCY_B  notchFreqB,
              bool             enableNotchFilterB    = true,
              bool             enableLowpassFilterB  = true,
              bool             enableHighpassFilterB = true
             );

    // Méthode pour mettre à jour le signal filtré
    int update(int inputValueB);

  private:
    SAMPLE_FREQUENCY_B m_sampleFreqB;
    NOTCH_FREQUENCY_B  m_notchFreqB;
    bool             m_bypassEnabledB;
    bool             m_notchFilterEnabledB;
    bool             m_lowpassFilterEnabledB;
    bool             m_highpassFilterEnabledB;
};

#endif
