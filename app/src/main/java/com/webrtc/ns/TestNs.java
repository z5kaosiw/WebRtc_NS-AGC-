package com.webrtc.ns;

import android.app.Activity;
import android.util.Log;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.nio.ByteBuffer;
import java.nio.ShortBuffer;

/*

 这个是可以进行转换和复原的，举例：
short x = -32752;//定义一个short
byte high = (byte) (0x00FF & (x>>8));//定义第一个byte
byte low = (byte) (0x00FF & x);//定义第二个byte
System.out.println(high);//打印第一个byte值
System.out.println(low);//打印第二个byte值
// 复原
short z = (short)(((high & 0x00FF) << 8) | (0x00FF & low));
System.out.println(z);//输出的结果就是-32752

 * */
public class TestNs {

    public static final  String  AGC_OUT_FILE_PATCH_DICTORY="/storage/emulated/0/Pictures/ns";
    public static final  String  AGC_OUT_FILE_PATCH="/storage/emulated/0/Pictures/ns/byby_8K_1C_16bit_ns_android.pcm";

    public  static    void  ns_audio_test(Activity act)  {
        try {
            File file = null;
            InputStream is = act.getAssets().open("byby_8K_1C_16bit.pcm");
            FileOutputStream fos = null;
            file = new File(AGC_OUT_FILE_PATCH_DICTORY);
            if (!file.exists()){
                boolean mkdirs = file.mkdirs();
                if (mkdirs) {
                    log("create dictroy success");
                } else {
                    log("create dictroy file");
                    return;
                }

            }
            file = new File(AGC_OUT_FILE_PATCH);
            fos = new FileOutputStream(file);
            log("sleep 2s ");
            Thread.sleep(2000);

            long res = AudioProcessJni.initiateNSInstance(8000,3);
            if (res == 7 ){
                //success+

            }
            BytesTransUtil bytesTransUtil = BytesTransUtil.getInstance();
            byte[] rData = new byte[160];
            ByteBuffer outBuffer  =  ByteBuffer.allocateDirect(160);
            int nbread = 0;

            long startTime = System.currentTimeMillis();
            while((nbread=is.read(rData))>-1){

                short[] shorts = bytesTransUtil.Bytes2Shorts(rData);
                // ByteBuffer.wrap(buffer).order(ByteOrder.LITTLE_ENDIAN).asShortBuffer().get(data);
                  res = AudioProcessJni.processNS(outBuffer, shorts, 80);

                for (int i = 0 ;i< 80 ;i++){

                    shorts[i] = (short) ( (outBuffer.get(2*i)&0x00FF )  + ( outBuffer.get(2*i+1) << 8));
                }
               fos.write(bytesTransUtil.Shorts2Bytes(shorts),0,nbread);
            }

            long endTime = System.currentTimeMillis();
            log(" 结束 Ns = 程序运行时间："+(endTime-startTime)+"ms");

            if (fos!=null){

                fos.close();
            }

        } catch (IOException e) {
            e.printStackTrace();
            log("e:error -> "+e.toString());
        } catch (InterruptedException e) {
            e.printStackTrace();
        }finally {


        }


    }


    /**
     * 字节转十六进制
     * @param b 需要进行转换的byte字节
     * @return  转换后的Hex字符串
     */
    public static String byteToHex(byte b){
        String hex = Integer.toHexString(b & 0xFF);
        if(hex.length() < 2){
            hex = "0" + hex;
        }
        return hex;
    }


    /**
     * 字节数组转16进制
     * @param bytes 需要转换的byte数组
     * @return  转换后的Hex字符串
     */
    public static String bytesToHex(byte[] bytes) {
        StringBuffer sb = new StringBuffer();
        for(int i = 0; i < bytes.length; i++) {
            String hex = Integer.toHexString(bytes[i] & 0xFF);
            if(hex.length() < 2){
                sb.append(0);
            }
            sb.append(hex);
        }
        return sb.toString();
    }

    /**
     * hex字符串转byte数组
     * @param inHex 待转换的Hex字符串
     * @return  转换后的byte数组结果
     */
    public static byte[] hexToByteArray(String inHex){
        int hexlen = inHex.length();
        byte[] result;
        if (hexlen % 2 == 1){
            //奇数
            hexlen++;
            result = new byte[(hexlen/2)];
            inHex="0"+inHex;
        }else {
            //偶数
            result = new byte[(hexlen/2)];
        }
        int j=0;
        for (int i = 0; i < hexlen; i+=2){
            result[j]=hexToByte(inHex.substring(i,i+2));
            j++;
        }
        return result;
    }

    /**
     * Hex字符串转byte
     * @param inHex 待转换的Hex字符串
     * @return  转换后的byte
     */
    public static byte hexToByte(String inHex){
        return (byte)Integer.parseInt(inHex,16);
    }





    public static   void   log(String log){

        Log.e("Android_JNI",log);
    }


}
