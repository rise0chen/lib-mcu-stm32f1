#include "GPS.hpp"

GPS_INFO   GPS;
unsigned char GetComma(unsigned char num,char *str);/// 获取第几个逗号
static double Get_Double_Number(char *s);
static float Get_Float_Number(char *s);
static void UTC2BTC(DATE_TIME *GPS);

//====================================================================//
// 语法格式：int GPS_RMC_Parse(char *line, GPS_INFO *GPS)
// 实现功能： 把gps模块的GPRMC信息解析为可识别的数据
// 参    数：存放原始信息字符数组、存储可识别数据的结构体
// 返 回 值：
//          1: 解析GPRMC完毕
//          0: 没有进行解析，或数据无效
//====================================================================//
int GPS_RMC_Parse(char *line, GPS_INFO *GPS)
{
    unsigned char ch, status, tmp;
    float lati_cent_tmp, lati_second_tmp;
    float long_cent_tmp, long_second_tmp;
    float speed_tmp;
    char *buf = line;/// 得到缓存
    ch = buf[5];
    status = buf[GetComma(2, buf)];

    if (ch == 'C')/// 如果第五个字符是C，($GPRMC)
    {
        if (status == 'A')/// 如果数据有效，则分析已经定位
        {
            GPS -> NS       = buf[GetComma(4, buf)];
            GPS -> EW       = buf[GetComma(6, buf)];

            GPS->latitude   = Get_Double_Number(&buf[GetComma(3, buf)]);
            GPS->longitude  = Get_Double_Number(&buf[GetComma( 5, buf)]);

            GPS->latitude_Degree  = (int)GPS->latitude / 100;/// 分离纬度
            lati_cent_tmp         = (GPS->latitude - GPS->latitude_Degree * 100);
            GPS->latitude_Cent    = (int)lati_cent_tmp;
            lati_second_tmp       = (lati_cent_tmp - GPS->latitude_Cent) * 60;
            GPS->latitude_Second  = (int)lati_second_tmp;

            GPS->longitude_Degree = (int)GPS->longitude / 100;/// 分离经度
            long_cent_tmp         = (GPS->longitude - GPS->longitude_Degree * 100);
            GPS->longitude_Cent   = (int)long_cent_tmp;
            long_second_tmp       = (long_cent_tmp - GPS->longitude_Cent) * 60;
            GPS->longitude_Second = (int)long_second_tmp;

            speed_tmp      = Get_Float_Number(&buf[GetComma(7, buf)]);/// 速度(单位：海里/时)
            GPS->speed     = speed_tmp * 1.85; /// 1海里=1.85公里
            GPS->direction = Get_Float_Number(&buf[GetComma(8, buf)]);/// 角度

            GPS->D.hour    = (buf[7] - '0') * 10 + (buf[8] - '0');/// 时间
            GPS->D.minute  = (buf[9] - '0') * 10 + (buf[10] - '0');
            GPS->D.second  = (buf[11] - '0') * 10 + (buf[12] - '0');
            tmp = GetComma(9, buf);
            GPS->D.day     = (buf[tmp + 0] - '0') * 10 + (buf[tmp + 1] - '0');//日期
            GPS->D.month   = (buf[tmp + 2] - '0') * 10 + (buf[tmp + 3] - '0');
            GPS->D.year    = (buf[tmp + 4] - '0') * 10 + (buf[tmp + 5] - '0')+2000;

            UTC2BTC(&GPS->D);

            return 1;
        }
    }

    return 0;
}


//====================================================================//
// 语法格式：int GPS_GGA_Parse(char *line, GPS_INFO *GPS)
// 实现功能：把gps模块的GPGGA信息解析为可识别的数据
// 参    数：存放原始信息字符数组、存储可识别数据的结构体
// 返 回 值：
//	     1: 解析GPGGA完毕
//           0: 没有进行解析，或数据无效
//====================================================================//
int GPS_GGA_Parse(char *line,GPS_INFO *GPS)
{
    unsigned char ch, status;
    char *buf = line;
    ch = buf[4];
    status = buf[GetComma(2, buf)];

    if (ch == 'G')//$GPGGA
    {
        if (status != ',')
        {
            GPS->height_sea = Get_Float_Number(&buf[GetComma(9, buf)]);
            GPS->height_ground = Get_Float_Number(&buf[GetComma(11, buf)]);

            return 1;
        }
    }

    return 0;
}

//====================================================================//
// 语法格式: static float Str_To_Float(char *buf)
// 实现功能： 把一个字符串转化成浮点数
// 参    数：字符串
// 返 回 值：转化后单精度值
//====================================================================//
static float Str_To_Float(char *buf)
{
    float rev = 0;
    float dat = 0;
    int integer = 1;
    char *str = buf;
    int i = 0;
    while(*str != '\0')
    {
        switch(*str)
        {
        case '0':
            dat = 0;
            break;
        case '1':
            dat = 1;
            break;
        case '2':
            dat = 2;
            break;
        case '3':
            dat = 3;
            break;
        case '4':
            dat = 4;
            break;
        case '5':
            dat = 5;
            break;
        case '6':
            dat = 6;
            break;
        case '7':
            dat = 7;
            break;
        case '8':
            dat = 8;
            break;
        case '9':
            dat = 9;
            break;
        case '.':
            dat = '.';
            break;
        }
        if(dat == '.')
        {
            integer = 0;
            i = 1;
            str ++;
            continue;
        }
        if( integer == 1 )
        {
            rev = rev * 10 + dat;
        }
        else
        {
            rev = rev + dat / (10 * i);
            i = i * 10 ;
        }
        str ++;
    }
    return rev;

}

//====================================================================//
// 语法格式: static float Get_Float_Number(char *s)
// 实现功能： 把给定字符串第一个逗号之前的字符转化成单精度型
// 参    数：字符串
// 返 回 值：转化后单精度值
//====================================================================//
static float Get_Float_Number(char *s)
{
    char buf[10];
    unsigned char i;
    float rev;
    i=GetComma(1, s);
    i = i - 1;
    strncpy(buf, s, i);
    buf[i] = 0;
    rev=Str_To_Float(buf);
    return rev;
}

//====================================================================//
// 语法格式: static double Str_To_Double(char *buf)
// 实现功能： 把一个字符串转化成浮点数
// 参    数：字符串
// 返 回 值：转化后双精度值
//====================================================================//
static double Str_To_Double(char *buf)
{
    double rev = 0;
    double dat;
    int integer = 1;
    char *str = buf;
    int i=0;
    while(*str != '\0')
    {
        switch(*str)
        {
        case '0':
            dat = 0;
            break;
        case '1':
            dat = 1;
            break;
        case '2':
            dat = 2;
            break;
        case '3':
            dat = 3;
            break;
        case '4':
            dat = 4;
            break;
        case '5':
            dat = 5;
            break;
        case '6':
            dat = 6;
            break;
        case '7':
            dat = 7;
            break;
        case '8':
            dat = 8;
            break;
        case '9':
            dat = 9;
            break;
        case '.':
            dat = '.';
            break;
        }
        if(dat == '.')
        {
            integer = 0;
            i = 1;
            str ++;
            continue;
        }
        if( integer == 1 )
        {
            rev = rev * 10 + dat;
        }
        else
        {
            rev = rev + dat / (10 * i);
            i = i * 10 ;
        }
        str ++;
    }
    return rev;
}

//====================================================================//
// 语法格式: static double Get_Double_Number(char *s)
// 实现功能：把给定字符串第一个逗号之前的字符转化成双精度型
// 参    数：字符串
// 返 回 值：转化后双精度值
//====================================================================//
static double Get_Double_Number(char *s)
{
    char buf[10];
    unsigned char i;
    double rev;
    i=GetComma(1, s);
    i = i - 1;
    strncpy(buf, s, i);
    buf[i] = 0;
    rev=Str_To_Double(buf);
    return rev;
}

//====================================================================//
// 语法格式：static unsigned char GetComma(unsigned char num,char *str)
// 实现功能：计算字符串中各个逗号的位置
// 参    数：查找的逗号是第几个的个数，需要查找的字符串
// 返 回 值：0
//====================================================================//


unsigned char GetComma(unsigned char num,char *str)
{
    unsigned char i,j = 0;
    int len=strlen(str);
    for(i = 0;i < len;i ++)
    {
        if(str[i] == ',')
            j++;
        if(j == num)
            return i + 1;
        if(j > GPS_MAX_COMMA)//如果超时
            return 0;
    }

    return 0;
}

//====================================================================//
// 语法格式：void UTC2BTC(DATE_TIME *GPS)
// 实现功能：转化时间为北京时区的时间
// 参    数：存放时间的结构体
// 返 回 值：无
//====================================================================//
static void UTC2BTC(DATE_TIME *GPS)
{
    GPS->second ++;
    if(GPS->second > 59)
    {
        GPS->second = 0;
        GPS->minute ++;
        if(GPS->minute > 59)
        {
            GPS->minute = 0;
            GPS->hour ++;
        }
    }

    GPS->hour = GPS->hour + 8;
    if(GPS->hour > 23)
    {
        GPS->hour -= 24;
        GPS->day += 1;
        if(GPS->month == 2 ||
           GPS->month == 4 ||
           GPS->month == 6 ||
           GPS->month == 9 ||
           GPS->month == 11 )
        {
            if(GPS->day > 30)
            {
                GPS->day = 1;
                GPS->month++;
            }
        }
        else
        {
            if(GPS->day > 31)
            {
                GPS->day = 1;
                GPS->month ++;
            }
        }
        if(GPS->year % 4 == 0 )
        {
            if(GPS->day > 29 && GPS->month == 2)
            {
                GPS->day = 1;
                GPS->month ++;
            }
        }
        else
        {
            if(GPS->day > 28 &&GPS->month == 2)
            {
                GPS->day = 1;
                GPS->month ++;
            }
        }
        if(GPS->month > 12)
        {
            GPS->month -= 12;
            GPS->year ++;
        }
    }
}
//====================================================================//
//	语法格式：	Int_To_Str(int x,char *Str)
//	实现功能：	转化整型值为字符串形式
//	参数：		x: 转化的整数
//				Str:转化后的字符串
//	返回值：	无
//====================================================================//
void Int_To_Str(int x,char *Str)
{
    int t;
    char *Ptr,Buf[5];
    int i = 0;
    Ptr = Str;
    if(x < 10)		// 当整数小于10时,转化为"0x"的格式
    {
        *Ptr ++ = '0';
        *Ptr ++ = x+0x30;
    }
    else
    {
        while(x > 0)
        {
            t = x % 10;
            x = x / 10;
            Buf[i++] = t+0x30;	// 通过计算把数字转化成ASCII码形式
        }
        i -- ;
        for(;i >= 0;i --) 		// 将得到的字符串倒序
        {
            *(Ptr++) = Buf[i];
        }
    }
    *Ptr = '\0';
}

//39.05.55 117.08.53
#define POSITION_CHAR_LEN 30
char position[POSITION_CHAR_LEN] = {'0'};
char *GPS_DisplayOne()
{
    char info[5];
    memset(position,0,POSITION_CHAR_LEN);
    strcat(position,"Lat:");
    Int_To_Str(GPS.latitude_Degree,info);//纬度
    strcat(position,info);
    strcat(position,".");
    Int_To_Str(GPS.latitude_Cent,info);//纬分
    strcat(position,info);
    strcat(position,".");
    Int_To_Str(GPS.latitude_Second,info);//纬秒
    strcat(position,info);
    strcat(position,"  Lon:");
    Int_To_Str(GPS.longitude_Degree,info);//经度
    strcat(position,info);
    strcat(position,".");
    Int_To_Str(GPS.longitude_Cent,info);//经分
    strcat(position,info);
    strcat(position,".");
    Int_To_Str(GPS.longitude_Second,info);//经秒
    strcat(position,info);

    return position;
}

char *Get_Position_Simple()
{
    if(strlen(position)==0)
        return NULL;
    return position;
}
