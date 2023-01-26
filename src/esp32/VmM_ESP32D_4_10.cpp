#include<ELi_VmM_4_10.h>
#include<ELi_McM_4_00.h>
#include<Wire.h>
#include <math.h>

VmM_4_10::VmM_4_10(int address=22)
{
  VmM_chgBoardadd(address);
};
int VmM_4_10::VmM_chgBoardadd(int address)
      {
        switch (address)
        {
            case 11: this->addr = 0b0100000;break;
            case 21: this->addr = 0b0100010;break;
            case 01: this->addr = 0b0100011;break;
            case 12: this->addr = 0b0101000;break;
            case 22: this->addr = 0b0101010;break;
            case 02: this->addr = 0b0101011;break;
            case 10: this->addr = 0b0101100;break;
            case 20: this->addr = 0b0101110;break;
            case 00: this->addr = 0b0101111;break;
            default: this->addr=0;break;
        }
      }
int VmM_4_10::VmM_sendi2cdata(){
          int i=0;
          Wire.begin(SDA, SCL);
          Wire.beginTransmission((int)(this->addr));
          for(i=0;i<2;i++){
          Wire.write(txbuffer[i]); 
           }
          Wire.endTransmission();
        }

int VmM_4_10::VmM_Command16(){
        txbuffer[0] = 0B11010000;
        txbuffer[1] = 0B00000011;
        VmM_sendi2cdata();  
        VmM_check_i2c(2); 
          
}
int VmM_4_10::VmM_check_i2c(int data){
        txbuffer[0] = 0b00110000;
        txbuffer[1] = data;
        VmM_sendi2cdata();
        Wire.requestFrom((int)(this->addr), 1);
        int VmM_read_bit=Wire.read();
        VmM_read_data = ((VmM_read_bit == 19) || (VmM_read_bit == 3))? 1:0;
}
int VmM_4_10::VmM_Write(bool RDACno, int D_set){   
        VmM_Command16();
        d_bit = D_set;
        txbuffer[0] = (RDACno)?0b00010001:0b00010000;
        txbuffer[1] = (d_bit);
        VmM_sendi2cdata();
}  
int VmM_4_10::VmM_StoreValue (bool RDACno){     
        VmM_Command16();
        d_bit = 1;  
        txbuffer[1] = d_bit;
        txbuffer[0] = (RDACno)?0b01110001:0b01110000;
        VmM_sendi2cdata();
    }
   
int VmM_4_10::VmM_RestoreValue (bool RDACno){   
        VmM_Command16();
        d_bit = 0;
        txbuffer[1] = d_bit;
        txbuffer[0] = (RDACno)?0b01110001:0b01110000;
        VmM_sendi2cdata();
    }

float VmM_4_10::setGain(float Gain_dB){
        if (Gain_dB>28.43){this-> D_1 = 255;this-> D_2 = 0;this-> Gain_dB_val = Max_dB_val;VmM_Write(0,this->D_1);VmM_Write(1,this->D_2);return (VmM_read_data)? 1:2;}
        else if (Gain_dB<-40.46){this-> D_1 = 0;this-> D_2 = 255;this-> Gain_dB_val = Min_dB_val;VmM_Write(0,this->D_1);VmM_Write(1,this->D_2);return (VmM_read_data)? -1:2;}
        else{
                
                float VmM_Gain_dB = Gain_dB;
                float VmM_Gain = pow(10,(VmM_Gain_dB/20));
                for(int D1 = 0;D1<256;D1++){
                        D2 = round(256-(256*(((VmM_Gain*VmM_RT)/(1+2*(VmM_RC/(((256.0- D1)/256.0)*VmM_RT+VmM_RW))))-VmM_RW)/VmM_RT));
                        if(D2<256 && D2>=0){
                                VmM_NGain_1dB = VmM_GainCal(D1,D2);
                                VmM_Gain_1dB= round(VmM_Gain_dB* 10) / 10; 
                                if(VmM_NGain_1dB == VmM_Gain_1dB){
                                        this-> D_1 = D1;
                                        this-> D_2 = D2;
                                        this-> Gain_dB_val = VmM_NGain_dB;
                                        break;
                                }
                        }
                }
                VmM_Write(0,this->D_1);
                VmM_Write(1,this->D_2);
                return (VmM_read_data)? 0:2;
                }
}  
float VmM_4_10::amplify(float amp_factor){float Amp_NGain=(this->Gain_dB_val)*amp_factor;float t= setGain(Amp_NGain);return t;}
float VmM_4_10::attenuate(float att_factor){float Att_NGain=(this->Gain_dB_val)/att_factor;float f= setGain(Att_NGain);return f;}
void VmM_4_10::storeGain (){
        VmM_StoreValue (0);
        VmM_StoreValue (1);
        this->ND_1=this->D_1;
        this->ND_2=this->D_2;
        this-> NGain_dB_val=this-> Gain_dB_val;
}
void VmM_4_10::restoreGain (){
        VmM_RestoreValue (0);
        VmM_RestoreValue (1);
        this->D_1=this->ND_1;
        this->D_2=this->ND_2;
        this-> Gain_dB_val = this-> NGain_dB_val;
}  
float VmM_4_10::incGain(float inc_dB){
        float Inc_Gain_dB_val = (this->Gain_dB_val) + inc_dB;
        float w = setGain(Inc_Gain_dB_val);
        return w ;
}  
float VmM_4_10::decGain(float dec_dB){
        float Dec_Gain_dB_val = (this->Gain_dB_val) - dec_dB;
        float x= setGain(Dec_Gain_dB_val);
        return x;
}
float VmM_4_10::VmM_GainCal(uint8_t D1,uint8_t D2){
        VmM_RWB = (((256.0- D2)/256.0)*VmM_RT+VmM_RW);
        VmM_RAW = (((256.0- D1)/256.0)*VmM_RT+VmM_RW);
        VmM_NGain = (VmM_RWB/VmM_RT)*(1+(2*(VmM_RC)/VmM_RAW));
        VmM_NGain_dB = (20*log10(VmM_NGain));
        VmM_NGain_1dB = round(VmM_NGain_dB* 10) / 10;
        return VmM_NGain_1dB;
}
float VmM_4_10::presentGain(){
        this->D_1 = VmM_Present_val(3,0);
        this->D_2 = VmM_Present_val(3,1);
        this->Gain_dB_val = VmM_GainCal(D_1,D_2);
        return this->Gain_dB_val;
}
int VmM_4_10::VmM_Present_val(int data,bool RDACno){
        txbuffer[0] = (RDACno)?0b00110001:0b00110000;
        txbuffer[1] = data;
        VmM_read_data = VmM_receivei2cdata();
        return VmM_read_data;
}
int VmM_4_10::VmM_receivei2cdata()
        {
          VmM_sendi2cdata();
          Wire.requestFrom((int)(this->addr), 1);
          int VmM_read_bit=Wire.read();
          return VmM_read_bit ;
        }

                             
