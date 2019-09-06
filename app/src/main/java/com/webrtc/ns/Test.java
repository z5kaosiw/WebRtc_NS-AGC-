package com.webrtc.ns;

import android.app.Activity;
import android.util.Log;

import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;

public class Test {


    public static final  String  AGC_OUT_FILE_PATCH_DICTORY="/storage/emulated/0/Pictures/ns/byby_8K_1C_16bit.pcm";
    public static final  String  AGC_DATA_SEET_PATCH="/storage/emulated/0/Pictures/ns/android_ns.txt";

    public static void Test(Activity act){


        try {


            InputStream is = act.getAssets().open("byby_8K_1C_16bit.pcm");
            BytesTransUtil bytesTransUtil = BytesTransUtil.getInstance();
            byte[] rData = new byte[160];
            int nbread = 0;
            while((nbread=is.read(rData))>-1){

                short[] shorts = bytesTransUtil.Bytes2Shorts(rData);

                for (int i = 0 ;i< 5;i++){

                        log("java log  i = "+ i +"  data:"+ shorts[i]);

                }

                int res = AudioProcessJni.testShortDataJni(shorts);

                Thread.sleep(5000);

                // fos.write(bytesTransUtil.Shorts2Bytes(shorts),0,nbread);
            }


        } catch (IOException e) {
            e.printStackTrace();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }


    }




    public static   void   log(String log){

        Log.e("Android_JNI",log);
    }


}
