package com.webrtc.ns;

import android.app.Activity;
import android.util.Log;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.nio.ByteBuffer;

public class TestAgc {

    public static final  String  AGC_OUT_FILE_PATCH_DICTORY="/storage/emulated/0/Pictures/agc";
    public static final  String  AGC_OUT_FILE_PATCH="/storage/emulated/0/Pictures/agc/byby_8K_1C_16bit_agc.pcm";
    public static    void  agc_audio_test(Activity  act)  {
        try {
            int nbread = 0;
            //读取Assets文件流
            InputStream is = act.getAssets().open("byby_8K_1C_16bit.pcm");

            //输出的文件目录
            File  file = new File(AGC_OUT_FILE_PATCH_DICTORY);
            if (!file.exists()){
                boolean mkdirs = file.mkdirs();
                if (mkdirs) {
                    log("create dictroy success");
                } else {
                    log("create dictroy file");
                    return;
                }
            }
            //输出的文件
            file = new File(AGC_OUT_FILE_PATCH);
            //调用初始刷Agc模块
            long res = AudioProcessJni.AgcInit(0, 255, 8000);
            log(" AudioProcessJni.AgcInit  res = "+ res);
            log("sleep 2s ");
            Thread.sleep(2000);
            //初始化byte转换工具
            BytesTransUtil bytesTransUtil = BytesTransUtil.getInstance();
            // rData 为读取的缓冲区 分配160字节
            byte[] rData = new byte[160];
            ByteBuffer  outBuffer = ByteBuffer.allocateDirect(160);
            FileOutputStream  fos = new FileOutputStream(file);
            //--------------------开始读取---------------------------
            while((nbread=is.read(rData))>-1){
                short[] shorts = bytesTransUtil.Bytes2Shorts(rData);
                res = AudioProcessJni.AgcFun(outBuffer, shorts,80);
                for (int i = 0 ;i< 80 ;i++){

                    shorts[i] = (short) (outBuffer.get(2*i) + ( outBuffer.get(2*i+1) << 8));
                }
                  fos.write(bytesTransUtil.Shorts2Bytes(shorts),0,nbread);
            }
            log(" 结束Agc = " );
            if (fos!=null){

                fos.close();
            }
        } catch (IOException e) {
            e.printStackTrace();
            log("e:error -> "+e.toString());
        } catch (InterruptedException e) {
            e.printStackTrace();
        }finally {

            AudioProcessJni.AgcFree();
        }
    }
    public static   void   log(String log){

        Log.e("Android_JNI",log);
    }

}
