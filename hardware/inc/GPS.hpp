#ifndef __GPS_H
#define __GPS_H

#define GPS_ERR "gps parser time out ... \r\n"
#define GPS_DEBUG 1
#define GPS_MAX_COMMA 256
#define tpy_beiwei     = "北纬";
#define tpy_nanwei     = "南纬";
#define tpy_dongjing   = "东经";
#define tpy_xijing     = "西经";

#define tpy_sudu       = "速度: ";
#define tpy_hangxiang  = "航向: ";
#define tpy_gaodu      = "高度: ";
#define tpy_jiaodu     = "角度: ";
#define tpy_haiba      = "海拔: ";
#define tpy_du         = "度";
#define tpy_meter      = "米";
#define tpy_kmperhour  = "km/h";
#define tpy_date       = "    年  月  日  ";

typedef struct{
    int year;
    int month;
    int  day;
    int hour;
    int minute;
    int second;
}DATE_TIME;

typedef struct{
    double  latitude;                       //经度
    double  longitude;                      //纬度
    int     latitude_Degree;                //度
    int	    latitude_Cent;                  //分
    int     latitude_Second;                //秒
    int     longitude_Degree;               //度
    int	    longitude_Cent;                 //分
    int     longitude_Second;               //秒
    float   speed;                          //速度
    float   direction;                      //航向
    float   height_ground;                  //水平面高度
    float   height_sea;                     //海拔高度
    unsigned char NS;
    unsigned char EW;
    DATE_TIME D;
}GPS_INFO;

extern int GPS_RMC_Parse(char *line,GPS_INFO *GPS);
extern int GPS_GGA_Parse(char *line,GPS_INFO *GPS);
extern void Int_To_Str(int x,char *Str);
extern GPS_INFO   GPS;
extern char *GPS_DisplayOne(void);
extern char *Get_Position_Simple(void);

extern unsigned char GetComma(unsigned char num,char *str);
#endif
