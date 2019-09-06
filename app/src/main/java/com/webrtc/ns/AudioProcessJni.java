package com.webrtc.ns;

import java.nio.ByteBuffer;
/*
*   先消回声再降噪效果比先降噪再消回声好 建议参考WebRTC AudioPreprocessing 模块里面的 ProcessStream 的实现顺序。
* */
public class AudioProcessJni {

     static{
          System.loadLibrary("webrtc_audio");  //加载native code的动态库
     }
     //音频降噪接口
     //mode:4种模式分别对应：0/1/2/3，数值越高，效果越明显
     public   native  static  int   initiateNSInstance(long sf,int mode);
     public   native  static  int   processNS32K(ByteBuffer buffer1,short  outframe[],int sf);
     public   native  static  int   processNS(ByteBuffer buffer1,short  outframe[],int sf);
     public   native  static  void  NSProcess(int sf);  //ns test  model
     public   native  static  int   destoryNS();
     //Audio agc音频增益接口
     public   native  static  void  AgcFree();
     public   native  static  int   AgcFun(ByteBuffer buffer1 , short[] sArr, int frameSize);
     public   native  static  long  AgcInit(long minLevel, long maxLevel,long fs);
     public   native  static  void  AgcProcess();  //agc test  model

     //音频回声消除接口



}
