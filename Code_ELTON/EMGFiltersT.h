#ifndef _EMGFILTERS_T_H
#define _EMGFILTERS_T_H

enum NOTCH_FREQUENCY_T { NOTCH_FREQ_50HZ_T = 50, NOTCH_FREQ_60HZ_T = 60 };

enum SAMPLE_FREQUENCY_T { SAMPLE_FREQ_500HZ_T = 500, SAMPLE_FREQ_1000HZ_T = 1000 };

// Déclaration de la classe EMGFilters
class EMGFiltersT {
  public:
    // Méthode pour initialiser les filtres EMG
    void init(SAMPLE_FREQUENCY_T sampleFreqT,
              NOTCH_FREQUENCY_T  notchFreqT,
              bool             enableNotchFilterT    = true,
              bool             enableLowpassFilterT  = true,
              bool             enableHighpassFilterT = true
             );

    // Méthode pour mettre à jour le signal filtré
    int update(int inputValueT);

  private:
    SAMPLE_FREQUENCY_T m_sampleFreqT;
    NOTCH_FREQUENCY_T  m_notchFreqT;
    bool             m_bypassEnabledT;
    bool             m_notchFilterEnabledT;
    bool             m_lowpassFilterEnabledT;
    bool             m_highpassFilterEnabledT;
};

#endif
