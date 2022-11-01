#ifndef VmM_ESP32D_4_10_H
#define VmM_ESP32D_4_10_H

#include <ELi_McM_4_10.h>
#include<stdint.h>


class VmM_4_10{
    private:
        unsigned addr,d_bit;
        int VmM_RT = 10000,VmM_RC = 1000,VmM_RW = 40;
        float VmM_NGain_1dB,VmM_NGain_dB,VmM_NGain,VmM_Gain_1dB,VmM_RWB,VmM_RAW,D2;
        uint8_t txbuffer[8],D_1,D_2,ND_1,ND_2;
        float Gain_dB_val,NGain_dB_val,Max_dB_val=28.43,Min_dB_val=-40.46;
        int VmM_read_data;
        int VmM_sendi2cdata();
        int VmM_received_data();
        int VmM_Command16();
        float VmM_GainCal(uint8_t,uint8_t);
        int VmM_Present_val(int ,bool);
        int VmM_Write(bool, int);
        int VmM_StoreValue (bool);
        int VmM_RestoreValue (bool);
        int VmM_chgBoardadd(int);
        int VmM_check_i2c(int);
        int VmM_receivei2cdata();
    public:   
        VmM_4_00(int);
        float setGain(float);
        void storeGain ();
        void restoreGain ();
        float incGain(float);
        float decGain(float);
        float amplify(float);
        float attenuate(float);
        float presentGain();
};

#endif
