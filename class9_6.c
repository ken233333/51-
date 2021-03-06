// 具备启动K4停止K8电机运动功能
// 能加速K5 减速K6 
// 初始档位为50
// 加入液晶显示
// 转速读取，并做液晶显示
// 把转速信号连续最近的三次做平均
// 把调占空比的方式改成调转速的输出控制方式
// 根据当前转速和目标转速缓慢调整占空比，逐次逼近
// 使用比例环调节
//

#include<reg51.h>
#include"lcd.h"
#define uchar unsigned char
#define uint unsigned int
#define time 1000
sbit motor_a=P1^0;
sbit motor_b=P1^1;
sbit key_run=P1^4;
sbit key_stp=P1^7;
sbit spd_up=P1^5;
sbit spd_dn=P1^6;    //T0口 定时  T1口计数

bit key_run_last=1,key_run_now=1;
bit key_stp_last=1,key_stp_now=1;
bit spd_up_last=1,spd_up_now=1;
bit spd_dn_last=1,spd_dn_now=1;

uchar status,count_zq;
uchar set_zkb=0;
uchar set_spd=40;
uchar qian_wei,bai_wei,shi_wei,ge_wei;
uint sped;
uint sped1,sped2,sped3,avr_spd;
uint sd_count;

void run();
void stop();
void scan_key();
void int_t0();
void fenjie(int j);
void disp();
void main()
{
  TMOD=0x51;
  EA=1;
  ET0=1;
  lcd_init();
  TH1=0;
  TL1=0;
  TR1=1;
  while(1)
  {
    scan_key();
	avr_spd=(sped1+sped2+sped3)/3;
	disp();
  }
}

void run()
{
  motor_a=1;
  motor_b=0;
}

void stop()
{
  motor_a=1;
  motor_b=1;
}

void scan_key()
{
  key_run_last=key_run_now;
  key_run_now = key_run;

  key_stp_last=key_stp_now;
  key_stp_now = key_stp;

  spd_up_last=spd_up_now;
  spd_up_now = spd_up;

  spd_dn_last=spd_dn_now;
  spd_dn_now = spd_dn;

  if(key_run_last==0&&key_run_now==1)
  {
	 status=1;
	 TH0=(65536-time)/256;
  	 TL0=(65536-time)%256;
	 TR0=1;
  }

  if(key_stp_last==0&&key_stp_now==1)
  {
	 status=0;
	 TR0=0;
	 stop();
  }

  if(spd_up_last==0&&spd_up_now==1&&status==1)
  {
	 set_spd++;
	 if(set_spd>=60)
	 set_spd=60;
  }
  if(spd_dn_last==0&&spd_dn_now==1&&status==1)
  {
	 set_spd--;
	 if(set_spd<=1)
	 set_spd=1;
  }
}

void int_t0() interrupt 1
{
    if(status==1)
	 {
		  count_zq++;
		  if(count_zq==100)
		   count_zq=0;
		  if(count_zq<set_zkb)
			run();
		  else 
		    stop();
	 }

	 sd_count++;
	 if(sd_count==1000)
	 {
	  sd_count=0;
	  sped=TH1*256+TL1;
	  sped1=sped2;
	  sped2=sped3;
	  sped3=sped;
	  TH1=0;
	  TL1=0;

	 if(avr_spd<set_spd)
	  set_zkb=set_zkb +(set_spd-avr_spd)/2;
	 if(avr_spd>set_spd)
	  set_zkb=set_zkb -(avr_spd-set_spd)/2;
	 }

	 TH0=(65536-time)/256;
  	 TL0=(65536-time)%256;
	 TR0=1;
}

void disp()
{
  if(status==1)
  {
	lcd_pos(1,1);
    lcdwrite_zm('r');
    lcdwrite_zm('u');
    lcdwrite_zm('n');
  }
  else
  {
	lcd_pos(1,1);
    lcdwrite_zm('s');
    lcdwrite_zm('t');
    lcdwrite_zm('p');
  }

  fenjie(set_zkb);
  lcd_pos(1,8);
    lcdwrite_zm('z');
    lcdwrite_zm('k');
    lcdwrite_zm('b');
  lcdwrite_sz(qian_wei);
  lcdwrite_sz(bai_wei);
  lcdwrite_sz(shi_wei);
  lcdwrite_sz(ge_wei);

  fenjie(avr_spd);
  lcd_pos(2,1);
    lcdwrite_zm('s');
    lcdwrite_zm('p');
    lcdwrite_zm('d');
  lcdwrite_sz(qian_wei);
  lcdwrite_sz(bai_wei);
  lcdwrite_sz(shi_wei);
  lcdwrite_sz(ge_wei);

  fenjie(set_spd);
  lcd_pos(2,9);
    lcdwrite_zm('s');
    lcdwrite_zm('e');
    lcdwrite_zm('t');
  lcdwrite_sz(qian_wei);
  lcdwrite_sz(bai_wei);
  lcdwrite_sz(shi_wei);
  lcdwrite_sz(ge_wei);
}

void fenjie(uint j)
{
  uint remain;
  qian_wei=j/1000;
  remain=j%1000;
  bai_wei=remain/100;
  remain=remain%100;
  shi_wei=remain/10;
  ge_wei=remain%10;
}